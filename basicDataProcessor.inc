<?php
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
// DATA PART BASED ON DOCTRINE

// Consider using a custom query
//

Class basicDataProcessor extends Doctrine_Record
{

    private $conn = null;

    function __construct($conn)
    {
        $this->conn = $conn;
    }

    function get_data_definitions($table, $fields)
    {
        // This interface is called by the renderer to find out the length and type of the database element so field size can be extablished
        // http://www.doctrine-project.org/projects/orm/1.2/docs/manual/component-overview/en

        $x_table_def = Doctrine_Core::getTable($table);
        $x_cols      = $x_table_def->getColumns();

        if (0) {
            print "<pre>";
            foreach ($x_cols as $x_name => $x_col) {
                print "NAME: $x_name\n";
                print_r($x_col);
                print "\n";
            }
            print "</pre>";
        }

        return $x_cols;
    }

    function __get_result_data_indexed_by_name($result, $column_names)
    {
        $x_a = array();

        // print "<pre>"; print_r($result->toArray()); print "</pre>";
        // print "<pre>"; print_r($column_names); print "</pre>";

        // process result row
        foreach ($result as $row) {
            $data_a = array();

            // process result col
            foreach ($column_names as $col) {
                // if we have a dot in the col we are looking for the result in a relation
                $dot_pos = strpos($col, '.');

                $relation = null;
                if ($dot_pos !== false AND $dot_pos > 0) {
                    $relation = substr($col, 0, $dot_pos);
                    $x_col    = strtolower(substr($col, $dot_pos + 1));
                    // print "<pre>"; print "$relation $col"; print "</pre>";
                }

                // If we are referring to a field in a relation process the relation and continue with the next col
                if ($relation !== null) {
                    if (isset($row->$relation->$x_col)) {
                        $data_a[$col] = $row->$relation->$x_col;
                    }
                    continue;
                }

                // The default is a non relation field
                $x_col = strtolower($col);
                if (isset($row->$x_col)) {
                    $data_a[$col] = $row->$x_col;
                }
            }
            $x_a[] = $data_a;
        }

        return $x_a;
    }

    function __add_order_by(&$q, $order_by_info = null)
    {
        $context = array(
            'File' => __FILE__,
            'Line' => __LINE__,
            'Func' => __FUNCTION__,
        );
        // print_debug($context,'$order_by_info',$order_by_info);
        // print "<pre> " ; print_r($order_by_info); print "</pre>";

        $order_by = array();

        if ($order_by_info !== null AND is_array($order_by_info)) {
            foreach ($order_by_info as $col_name => $col_value) {
                switch ($col_value) {
                    case 'ascending':
                        $order_by[] = "$col_name ASC";
                        break;

                    case 'descending':
                        $order_by[] = "$col_name DESC";
                        break;

                    default:
                        break;
                }
            }
        }

        if (count($order_by) > 0) {
            $order_by_s = implode(" , ", $order_by);
            $q->orderBy("$order_by_s");
        }

        return $q;
    }

    function __add_where_single($col_name, $col_value, &$where_a, &$where_b)
    {
        /////////////////////////////////////////
        // NOT

        $not  = '';
        $patt = '/^\s*!/';
        if (preg_match($patt, $col_value)) {
            $not = 'NOT';
            // remove the ! from the string
            $xx        = preg_replace($patt, '', $col_value);
            $col_value = $xx;
        }

        /////////////////////////////////////////
        // LIKE
        if (preg_match('/%/', $col_value)) {
            $where_a[] = "$col_name $not like ?";
            $where_b[] = $col_value;

            return;
        }

        /////////////////////////////////////////
        // REGEXP
        $patt = '/^\s*REGEXP:\s*/';
        if (preg_match($patt, $col_value)) {
            $type      = 'REGEXP';
            $xx        = preg_replace($patt, '', $col_value);
            $col_value = $xx;
            $where_a[] = "$col_name $not REGEXP ?";
            $where_b[] = $col_value;

            return;
        }

        /////////////////////////////////////////
        // GREATER SMALLER

        if (preg_match('/^>=(.*)/', $col_value, $matches)) {
            $where_a[] = "$col_name >= ?";
            $where_b[] = $matches[1];

            return;
        }

        if (preg_match('/^>(.*)/', $col_value, $matches)) {
            $where_a[] = "$col_name > ?";
            $where_b[] = $matches[1];

            return;
        }

        if (preg_match('/^<=(.*)/', $col_value, $matches)) {
            $where_a[] = "$col_name <= ?";
            $where_b[] = $matches[1];

            return;
        }

        if (preg_match('/^<(.*)/', $col_value, $matches)) {
            $where_a[] = "$col_name < ?";
            $where_b[] = $matches[1];

            return;
        }

        $where_a[] = "$col_name = ?";
        $where_b[] = $col_value;

        return;
    }

    function __add_where($configDataBrowser, &$q, $where_info = null)
    {
        $f = __FUNCTION__;

        // print "<pre>FUNCTION: $f " ; print_r($where_info); print "</pre>";

        $has_join = false;

        $Tables = null;
        if (isset($configDataBrowser['Browse']['Tables'])) {
            $Tables = $configDataBrowser['Browse']['Tables'];
        }
        if (count($Tables) > 1) {
            $has_join = true;
        }

        // This describes only the primary table
        $Table = $configDataBrowser['Browse']['Table'];
        $Class = $configDataBrowser['Browse']['Class'];
        $Alias = $configDataBrowser['Browse']['Alias'];

        $columns       = $configDataBrowser['Browse']['Columns'];
        $dummy_names_a = array_keys($columns);

        $where_a = array();
        $where_b = array();

        if ($where_info !== null AND is_array($where_info)) {
            foreach ($where_info as $col_name => $col_value) {

                $x_col_name = null;

                // print "<pre> WHERE: $col_name ==> $col_value </pre>";

                if (strpos($col_name, '.')) {
                    // We are processing a relation field, get the proper alias

                    $x_table = substr($col_name, 0, strpos($col_name, '.'));
                    $x_alias = null;
                    foreach ($Tables as $t_name => $t_def) {
                        if ($t_def['Class'] === $x_table) {
                            $x_alias = $t_def['Alias'];
                            break;
                        }
                    }

                    if ($x_alias !== null) {
                        // public string getSqlTableAlias(string componentAlias, string tableName)
                        $zz_alias = $q->getSqlTableAlias($x_alias, $x_table);
                        // print "<pre>XXXXX: $zz_alias </pre>";

                        $col_name = str_replace($x_table, $x_alias, $col_name);
                        // print "<pre> WHERE: $col_name ==> $col_value </pre>";
                    }
                }

                $patt = '/\s*,\s*/';
                if (preg_match($patt, $col_value)) {
                    $ww_a = preg_split($patt, $col_value);
                    foreach ($ww_a as $ww_item) {
                        $this->__add_where_single($col_name, $ww_item, &$where_a, &$where_b);
                    }
                    continue;
                }

                $this->__add_where_single($col_name, $col_value, &$where_a, &$where_b);
            }
        }

        if (count($where_a) > 0) {
            $where_s = implode(" AND ", $where_a);
            // print "<pre> XX where($where_s , $where_b); </pre>";
            $q->where("$where_s", $where_b);
        }

        return $q;
    }

    function get_query_count($configDataBrowser, $where_info = null)
    {
        $has_join = false;

        $Tables = null;
        if (isset($configDataBrowser['Browse']['Tables'])) {
            $Tables = $configDataBrowser['Browse']['Tables'];
        }

        if (count($Tables) > 1) {
            $has_join = true;
        }

        $Class = $configDataBrowser['Browse']['Class'];
        $Alias = $configDataBrowser['Browse']['Alias'];

        $q = Doctrine_Query::create()
            ->select('Count(*) as count')
            ->from("$Class $Alias");

        if ($has_join) {
            foreach ($Tables as $t_name => $t_def) {
                // print "<pre> HAS JOIN: $t_name </pre>";
                $t_class = $t_def['Class'];
                $t_alias = $t_def['Alias'];
                if ($t_class === $Class) {
                    continue;
                }

                if (0) {
                    print "<pre>Table: $t_name; Alias: $t_alias ; Class: $t_class; </pre>";
                }
                $q->leftJoin("$Alias.$t_class $t_alias");
            }
        }

        $this->__add_where($configDataBrowser, &$q, $where_info);

        if (0) {
            print "<pre> COUNT QUERY";
            print_r($q->getSqlParts());
            print $q->getSqlQuery();
            print "</pre>";
        }

        $domains = $q->execute();

        foreach ($domains as $domain) {
            $count = $domain->count;
        }

        return $count;
    }

    function autoRemember($where_info, $sort_info)
    {
        $here = current_url();

        if (isset($_SESSION['remember'][$here])) {

            $x_where = null;
            $x_sort  = null;

            $remember = $_SESSION['remember'][$here];

            if (isset($remember['where'])) {
                $x_where = $remember['where'];
            }
            if (isset($remember['sort'])) {
                $x_sort = $remember['sort'];
            }

            return array($x_where, $x_sort);
        } else {
            $remember                    = array(
                'where' => $where_info,
                'sort'  => $sort_info,
            );
            $_SESSION['remember'][$here] = $remember;

            return array($where_info, $sort_info);
        }
        // NOT REACHED
    }

    function get_basic_data($configDataBrowser, $where_info = null, $sort_info = null)
    {
        $has_join = false;

        $Tables = null;
        if (isset($configDataBrowser['Browse']['Tables'])) {
            $Tables = $configDataBrowser['Browse']['Tables'];
        }
        if (count($Tables) > 1) {
            $has_join = true;
        }

        // This describes only the primary table
        $Table = $configDataBrowser['Browse']['Table'];
        $Class = $configDataBrowser['Browse']['Class'];
        $Alias = $configDataBrowser['Browse']['Alias'];

        $columns       = $configDataBrowser['Browse']['Columns'];
        $dummy_names_a = array_keys($columns);

        $amount = $configDataBrowser['Browse']['Amount'];
        $offset = $configDataBrowser['Browse']['Offset'];
        $this->get_data_definitions($Table, null);

        // TODO: how to add relations here,
        // $configDataBrowser['Browse']['Class'] should be more complicated or differently implemented
        // mboot: 11-Dec-2010

        $x_select_a = array();
        $x_select_b = array();
        foreach ($dummy_names_a as $d_name) {
            if (isset($columns[$d_name]['colName'])) {
                $col_name = $columns[$d_name]['colName'];
            } else {
                $col_name = $d_name;
            }

            $x_alias = null;
            if (isset($columns[$d_name]['table'])) {
                $xx_table = $columns[$d_name]['table'];
                if ($xx_table !== $Table) {
                    // continue;
                } else {
                    $Alias = $Tables[$xx_table]['Alias'];
                }
            }
            // This means that there could be more columns displayed on screen than retrieved
            // The separation between col_dummy and col_name allows for named columns with identical data

            if (! in_array($col_name, $x_select_a)) {
                $x_select_a[] = $col_name;
                $x_select_b[] = $Alias . '.' . $col_name;
            }
        }
        $select_list_a = implode(',', $x_select_a);
        $select_list_b = implode(',', $x_select_b);

        $q = Doctrine_Query::create();
        // $q	->select($select_list_a);
        $q->from("$Class $Alias");

        if ($has_join) {
            foreach ($Tables as $t_name => $t_def) {
                // print "<pre> HAS JOIN: $t_name </pre>";
                $t_class = $t_def['Class'];
                $t_alias = $t_def['Alias'];
                if ($t_class === $Class) {
                    continue;
                }

                if (0) {
                    print "<pre>Table: $t_name; Alias: $t_alias ; Class: $t_class; </pre>";
                }
                $q->leftJoin("$Alias.$t_class $t_alias");
            }
        }

        $this->__add_where($configDataBrowser, $q, $where_info);
        $this->__add_order_by($q, $sort_info);

        $q->limit($amount);
        $q->offset($offset);

        if (0) {
            print "<pre> RESULTING DOCTRINE QUERY: ";
            $x_r   = $q->getSqlQuery();
            $x_r_a = explode(',', $x_r);
            print implode(",\n\t", $x_r_a);
            print "</pre>";
        }

        $result = $q->execute();

        // return $this -> __get_result_data_indexed_by_name($result,$column_names);
        return $this->__get_result_data_indexed_by_name($result, $x_select_a);
    }
}
