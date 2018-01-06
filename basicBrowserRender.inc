<?php

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
// HTML RENDERING OF THE DATA
// Currently the Fields are keyed by field name
// This prevents repeating the field as button
// Introduce a dummy key and move the field name to a attribure: 'colName' => '<the name of the column>',

Class basicBrowserRender
{
    // basicBrowserConfig
    private $basicDataProcessor = null;
    private $formTopSlot = null;
    private $formBottomSlot = null;
    private $formTopTableSlot = null;
    private $formBottomTableSlot = null;

    function __construct($conn)
    {
        $basicDataProcessor       = New basicDataProcessor($conn);
        $this->basicDataProcessor = $basicDataProcessor;
    }

    function __get_my_url()
    {
        $_me = $_SERVER['PHP_SELF'];

        // if there is index.php in there remove it
        if (preg_match('/\/index\.php/i', $_me)) {
            $_me = preg_replace('/\/index\.php/i', '', $_me);
        }

        return $_me;
    }

    function __render_search_fields($search_info)
    {
        if (0) {
            print "<pre>";
            print_r($search_info);
            print "</pre>";
        }

        $tables = null;
        if (isset($this->basicBrowserConfig['Browse']['Tables'])) {
            $tables = $this->basicBrowserConfig['Browse']['Tables'];
        }

        $table   = $this->basicBrowserConfig['Browse']['Table'];
        $columns = $this->basicBrowserConfig['Browse']['Columns'];

        // This now gets all the defs from getColumns (not sure about the relations yet)
        $yy_col_defs         = array();
        $yy_col_defs[$table] = $this->basicDataProcessor->get_data_definitions($table, $columns);


        $x_a   = array();
        $x_a[] = <<<EOS
			<tr>
EOS;

        if (0) {
            $f = __FUNCTION__;
            print "<pre>FUNCTION: $f ";
            print_r($columns);
            print "</pre>";
        }

        foreach ($columns as $col_dummy => $col_value) {

            $col_visible = true;
            if (isset($col_value['visible'])) {
                $col_visible = $col_value['visible'];
            }
            if ($col_visible === false) {
                continue;
            }

            $col_label      = $col_value['showLabel'];
            $col_searchable = $col_value['searchable'];

            if (isset($col_value['colName'])) {
                $col_name = $col_value['colName'];
            } else {
                $col_name = $col_dummy;
            }

            // NOTE: for now . is not allowed in auto xss clean so translate . to : or /
            $is_Relation = false;

            $x_col_name = $col_name;
            if (strpos($col_name, '.')) {
                // mixed str_replace ( mixed $search , mixed $replace , mixed $subject [, int &$count ] )
                $x_col_name  = str_replace('.', '/', $col_name);
                $is_Relation = true;
            }

            // print "<pre>$col_name</pre>";
            // print "<pre>$x_col_name</pre>";

            if ($col_searchable !== true) {
                $x_a[] = <<<EOS
			<td class="basicBrowserDataTable">
				&nbsp;
			</td>
EOS;
                continue;
            }

            $x_len = $this->basicBrowserConfig['Browse']['Columns'];
            $len   = 8;
            // Note for yy_col_defs this will not work yet as we have not yet adapted to relations; mboot 13-12-2010
            if (isset($yy_col_defs[$table][$col_name]['length'])) {
                $len = $yy_col_defs[$table][$col_name]['length'];

                if ($len > 15) {
                    $len = 15;
                }
                if ($len < 1) {
                    $len = 1;
                }
            }
            if (isset($this->basicBrowserConfig['Browse']['Columns'][$col_name]['length'])) {
                $len = $this->basicBrowserConfig['Browse']['Columns'][$col_name]['length'];
            }

            if ($len > 15) {
                $len = 15;
            }
            if ($len < 1) {
                $len = 1;
            }

            $search_field_data = '';
            if (isset($search_info[$col_name])) {
                $search_field_data = $search_info[$col_name];
            }
            // html_ent_escape ??

            $UserSearchFeedback['en'] = "
				use >, >=, <, <= for greater and smaller;
				use % for like or REGEXP: for regular expressions;
				use , to add extra conditions in one field
				e.g: >a, <n, %nj%
			";
            $lang                     = 'en';

            $x_a[] = <<<EOS
			<td class="basicBrowserDataTable">
				<input
					size="$len"
					type="text"
					name="xx_where[$x_col_name]"
					value="$search_field_data"
					title="{$UserSearchFeedback[$lang]}"
				/>
			</td>
EOS;
        }
        $x_a[] = <<<EOS
			</tr>
EOS;

        return implode("\n", $x_a);
    }

    function __render_button_rows()
    {
        $x_a = array();

        $table   = $this->basicBrowserConfig['Browse']['Table'];
        $columns = $this->basicBrowserConfig['Browse']['Columns'];

        $amount = $this->basicBrowserConfig['Browse']['Amount'];
        $offset = $this->basicBrowserConfig['Browse']['Offset'];
        $total  = $this->basicBrowserConfig['Browse']['Total'];

        $pages = ceil($total / $amount);
        // $page = ceil( ( $offset + 1 ) / $amount );

        $browse_actions1 = array(
            // Action => Label
            'SEARCH'    => 'SEARCH',
            'CLEAR_ALL' => 'CLEAR_ALL',
        );

        $browse_actions2 = array(
            'FIRST' => 'FIRST',
            'PREV'  => 'PREV',
            'NEXT'  => 'NEXT',
            'LAST'  => 'LAST',
        );

        $max_cols = count($columns);

        $x_a[] = <<<EOS

			<tr>
				<td class="basicBrowserDataTable"; colspan="$max_cols" style="text-align: center;">
EOS;

        //////////////////////////////////////////
        // Show Search and Clear
        foreach ($browse_actions1 as $action => $label) {
            $x_a[] = <<<EOS
					<span> <input type="submit" class="basicBrowserInputButton" name="BROWSE_ACTION[$action]" value="$label" /> </span>
EOS;
        }

        $x_a[] = <<<EOS

				</td>
			</tr>
			</tr>
			<tr>
				<td class="basicBrowserDataTable"; colspan="$max_cols" style="text-align: center;">
EOS;
        /////////////////////////////////////////
        // Show navigation only if there is something to navigate
        if ($pages > 1) {
            foreach ($browse_actions2 as $action => $label) {
                $x_a[] = <<<EOS
					<span> <input type="submit" class="basicBrowserInputButton" name="BROWSE_ACTION[$action]" value="$label" /> </span>
EOS;
            }
        }

        $x_a[] = <<<EOS
				</td>
			</tr>
EOS;

        return implode("\n", $x_a);
    }

    function __render_order_by_selectors($sort_info)
    {
        $x_a = array();

        $amount  = $this->basicBrowserConfig['Browse']['Amount'];
        $offset  = $this->basicBrowserConfig['Browse']['Offset'];
        $total   = $this->basicBrowserConfig['Browse']['Total'];
        $table   = $this->basicBrowserConfig['Browse']['Table'];
        $columns = $this->basicBrowserConfig['Browse']['Columns'];

        $sort_orders = array(
            ''           => '',
            'ascending'  => '(a)',
            'descending' => '(d)',
        );

        $x_a[] = <<<EOS
			<tr>
EOS;

        foreach ($columns as $col_dummy => $col_value) {

            $col_visible = true;
            if (isset($col_value['visible'])) {
                $col_visible = $col_value['visible'];
            }
            if ($col_visible === false) {
                continue;
            }

            $col_label    = $col_value['showLabel'];
            $col_sortable = $col_value['sortable'];

            if (isset($col_value['colName'])) {
                $col_name = $col_value['colName'];
            } else {
                $col_name = $col_dummy;
            }

            if ($col_sortable !== true) {
                $x_a[] = <<<EOS

			<th class="basicBrowserDataTable">
				$col_label
			</th>
EOS;
                continue;
            }

            $sort_order_field = '';
            $tail             = '';

            if (isset($sort_info[$col_dummy]) AND trim($sort_info[$col_dummy]) != '') {
                $zz = $sort_info[$col_dummy];
                if (in_array($zz, array_keys($sort_orders))) {
                    $sort_order_field = $sort_info[$col_dummy];
                    $tail             = $sort_orders[$sort_order_field];
                }
            }

            // Render the button for this column that allows for sorting
            $x_a[] = <<<EOS

			<th class="basicBrowserDataTable">
				<input type="submit" class="basicBrowserInputButton" name="BROWSE_ACTION[SORT][$col_dummy]" value="$col_label{$tail}" />
				<input type="hidden" name="sort_order[$col_dummy]" value="$sort_order_field" />
			</th>
EOS;
        }

        $x_a[] = <<<EOS
			</tr>
EOS;

        return implode("\n", $x_a);
    }

    function __render_table_page_size_changer()
    {
        $x_a = array();

        $amount   = $this->basicBrowserConfig['Browse']['Amount'];
        $columns  = $this->basicBrowserConfig['Browse']['Columns'];
        $max_cols = count($columns);

        $x_a[] = <<<EOS
			<tr>
				<td class="basicBrowserDataTable"; colspan="$max_cols" style="text-align: center;">
					Amount of lines per page
					<input type="text" size="4" maxlength="6" name="page_amount" value="$amount" />
				</td>
			</tr>
EOS;

        return implode("\n", $x_a);
    }

    function __test_field_has_callback($col_dummy)
    {
        $columns = $this->basicBrowserConfig['Browse']['Columns'];

        $bool = (
            isset($columns[$col_dummy]['islink']) AND is_array($columns[$col_dummy]['islink'])
            AND
            isset($columns[$col_dummy]['islink']['callback']) AND is_array($columns[$col_dummy]['islink']['callback'])
            AND
            isset($columns[$col_dummy]['islink']['callback']['function']) AND is_string($columns[$col_dummy]['islink']['callback']['function'])
            AND
            isset($columns[$col_dummy]['islink']['callback']['params']) AND is_array($columns[$col_dummy]['islink']['callback']['params'])
            AND
            // Note: the function has to be callable as memember of this object instance
            is_callable(array($this, $columns[$col_dummy]['islink']['callback']['function']))
        );

        // Both Function and Params has to be set but we only use Function at the time, Params is not yet evaluated

        return $bool;
    }

    function __render_the_data_one_cell($row, $col_dummy, $col_name)
    {
        $columns = $this->basicBrowserConfig['Browse']['Columns'];
        // later: if show_this_col in result === TRUE
        // This col_name indexes the real data retrieved from the database
        // If we have joins this data could not be here right now so test for availability: to be fixed when joins work; 13-Dec-2010;mboot
        $column_data = null;
        if (isset($row[$col_name])) {
            $column_data = $row[$col_name];
        }

        $display_data = $column_data; // the default way to render this data
        // now see if there is a special rendering available for this data only
        if ($this->__test_field_has_callback($col_dummy) === true) {

            // This col_name will be col_dummy (unique)
            $func   = $columns[$col_dummy]['islink']['callback']['function'];
            $params = $columns[$col_dummy]['islink']['callback']['params']; // not yet used
            // TODO: This is the real data and the cool_name, INVESTIGATE if this should be col_dummy, or both
            // For the moment we only pass the current col name and its data to the function
            // We could actually pass the entire data record indexed by tyhe lowercase col names

            $param_list   = array(
                '__ROW__' => $row,
                $col_name => $column_data,
            );
            $display_data = $this->$func($param_list);

            return $display_data;
        }

        // Allow others to render the data
        // render_func_<table>_<col_name> / note that this will make searching and where diffucult, try to use joins or database mappings
        // We may want to limit the data shown for fields like Text
        // TODO: limit option in the config

        return $display_data;
    }

    function __render_action_button($data)
    {
        return '';

        $x_a = array();
        // INSERT ACTION BUTTONS HERE
        // need PrimaryKey info per row
        $x_a[] = <<<EOS
		<td class="basicBrowserViewButton">
			<input type="submit" name="view[pk]" value="view" />
		</td>
EOS;

        return implode("\n", $x_a);
    }

    function __render_the_data_all($data)
    {
        $x_a = array();

        $amount = $this->basicBrowserConfig['Browse']['Amount'];
        $total  = $this->basicBrowserConfig['Browse']['Total'];

        $table   = $this->basicBrowserConfig['Browse']['Table'];
        $columns = $this->basicBrowserConfig['Browse']['Columns'];

        $pages = ceil($total / $amount);
        // $page = ceil( ( $offset + 1 ) / $amount );
        // RENDER DATA ROW
        foreach ($data as $row_index => $row) {
            $x_a[] = <<<EOS
			<tr>
EOS;
            // RENDER DATA COLUMN
            foreach ($columns as $col_dummy => $col_value) {
                $col_visible = true;
                if (isset($col_value['visible'])) {
                    $col_visible = $col_value['visible'];
                }
                if ($col_visible === false) {
                    continue;
                }

                if (isset($col_value['colName'])) {
                    $col_name = $col_value['colName'];
                } else {
                    $col_name = $col_dummy;
                }

                $display_data = $this->__render_the_data_one_cell($row, $col_dummy, $col_name);
                $x_a[]        = <<<EOS
				<td class="basicBrowserDataTable">
					$display_data
				</td>
EOS;
            }

            $x_a[] = $this->__render_action_button($data);
            $x_a[] = <<<EOS

			</tr>
EOS;
        }

        return implode("\n", $x_a);
    }

    function render_as_2d_table($sort_info, $search_info, $data)
    {
        $x_a = array();

        $title  = $this->basicBrowserConfig['Browse']['Title'];
        $amount = $this->basicBrowserConfig['Browse']['Amount'];
        $offset = $this->basicBrowserConfig['Browse']['Offset'];
        $total  = $this->basicBrowserConfig['Browse']['Total'];

        $table  = $this->basicBrowserConfig['Browse']['Table'];
        $tables = null;
        if (isset($this->basicBrowserConfig['Browse']['Tables'])) {
            $tables = $this->basicBrowserConfig['Browse']['Tables'];
        }
        $columns = $this->basicBrowserConfig['Browse']['Columns'];

        $pages = ceil($total / $amount);
        $page  = ceil(($offset + 1) / $amount);

        $_me = $this->__get_my_url();

        $ititle = '';

        $formTopSlot         = '';
        $formBottomSlot      = '';
        $formTopTableSlot    = '';
        $formBottomTableSlot = '';

        // Wrap the while browser in a for and hanle everything over the post interface
        // that is the most secure and makes life much easier on this side
        /////////////////////////////////////////////////////////
        $x_a[] = <<<EOS
		<div class="basicBrowser" >
			<div> $title </div>
			<form method="POST" action="$_me">

				$formTopSlot

				<input type="hidden" name="current_page" value="$page" />
				<input type="hidden" name="current_page_max" value="$pages" />

				<table class="basicBrowserDataTable" style="border-collapse: collapse;">

					$formTopTableSlot

EOS;

        // Each call wraps its own data in a <tr> .... </tr>
        $x_a[] = $this->__render_search_fields($search_info);
        $x_a[] = $this->__render_button_rows();
        $x_a[] = $this->__render_order_by_selectors($sort_info);
        $x_a[] = $this->__render_the_data_all($data);

        $x_a[] = $this->__render_table_page_size_changer();

        $x_a[] = <<<EOS

					$formBottomTableSlot

				</table>

				$formBottomSlot

			</form>

			<div>
				Total: $total ($pages Pages); Amount per Page: $amount; Current Offset: $offset (Page: $page);
			</div>
		</div>
EOS;

        return implode("\n", $x_a);
    }

    function get_new_offset($offset)
    {
        $amount   = $this->basicBrowserConfig['Browse']['Amount'];
        $x_offset = $this->basicBrowserConfig['Browse']['Offset'];
        $total    = $this->basicBrowserConfig['Browse']['Total'];
        $table    = $this->basicBrowserConfig['Browse']['Table'];
        $columns  = $this->basicBrowserConfig['Browse']['Columns'];

        if (! isset($_POST['BROWSE_ACTION'])) {
            if ($offset === null) {
                return $x_offset;
            }

            return $offset;
        }

        if (! is_array($_POST['BROWSE_ACTION'])) {
            if ($offset === null) {
                return $x_offset;
            }

            return $offset;
        }

        if (count($_POST['BROWSE_ACTION']) > 1) {
            if ($offset === null) {
                return $x_offset;
            }

            return $offset;
        }

        $action_tainted = array_keys($_POST['BROWSE_ACTION']);
        $action_tainted = $action_tainted[0];

        $pages = ceil($total / $amount);

        switch ($action_tainted) {

            case 'FIRST':
                $offset = 0;

                return $offset;
                break;

            case 'PREV':
            case 'NEXT':
                $tainted = $_POST['current_page'] * 1;

                $curr_page = $tainted;

                if ($curr_page < 1) {
                    return 0;
                }
                if ($curr_page > $pages) {
                    return 0;
                }
                $curr_offset = ($curr_page - 1) * $amount;
                if ($action_tainted === 'NEXT') {
                    $next_offset = $curr_offset + $amount;
                } else {
                    $next_offset = $curr_offset - $amount;
                }
                if ($next_offset < 0) {
                    $next_offset = 0;
                }
                if ($next_offset > (($pages - 1) * $amount)) {
                    $next_offset = ($pages - 1) * $amount;
                }

                return $next_offset;
                break;

            case 'LAST':
                $offset = ($pages - 1) * $amount;

                return $offset;
                break;

            case 'SEARCH':
            case 'CLEAR_ALL':
                return 0;
                break;

            case 'SORT':
                // if the user presses sort we start from the beginning of the new sort clause
                return 0;
                break;

            default:
                return 0;
                break;
        }
        // NOT REACHED
    }

    function process_where_fields(&$search_info, &$where_info)
    {
        if (0) {
            print "<pre>";
            print_r($_POST);
            print "</pre>";
        }

        $columns = $this->basicBrowserConfig['Browse']['Columns'];

        // NOTE: &$search_info,&$where_info
        // Are by reference explicitly

        if (! isset($_POST['BROWSE_ACTION'])) {
            return;
        }

        if (! is_array($_POST['BROWSE_ACTION'])) {
            return;
        }

        if (count($_POST['BROWSE_ACTION']) > 1) {
            return;
        }

        $action_tainted = array_keys($_POST['BROWSE_ACTION']);
        $action_tainted = $action_tainted[0];

        // process the where columns both for search and for navigation with the current where clause active
        // Explicitly clear when the clear button is pressed
        if (0) {
            print "<pre>";
            print_r($search_info);
            print "</pre>";
        }

        switch ($action_tainted) {

            case 'FIRST':
            case 'PREV':
            case 'NEXT':
            case 'LAST':
            case 'SEARCH':
            case 'SORT': // For sort we keep the same where clause, but we will add a order by clause
                foreach ($columns as $col_dummy => $value) {
                    if (isset($value['colName'])) {
                        $name = $value['colName'];
                    } else {
                        $name = $col_dummy;
                    }

                    $x_name = str_replace('.', '/', $name); // prepare for possible relation fields
                    // print "<pre> TEST_POST: $x_name // $name </pre>";

                    if (isset($_POST['xx_where'])
                        AND isset($_POST['xx_where'][$x_name])
                    ) {
                        if (trim($_POST['xx_where'][$x_name]) != '') {
                            // print "<pre> TAINTED $tainted </pre>";
                            $tainted = trim($_POST['xx_where'][$x_name]);

                            $search_info[$name] = $tainted; // for redisplay on screen
                            $where_info[$name]  = $tainted; // for database select
                        } else {
                            unset($search_info[$name]);
                            unset($where_info[$name]);
                        }
                    }
                }
                break;

            case 'CLEAR_ALL':
                $where_info  = null;
                $search_info = null;
                break;

            default:
                break;
        }
    }

    function process_sort_fields(&$sort_info)
    {
        $amount  = $this->basicBrowserConfig['Browse']['Amount'];
        $offset  = $this->basicBrowserConfig['Browse']['Offset'];
        $total   = $this->basicBrowserConfig['Browse']['Total'];
        $table   = $this->basicBrowserConfig['Browse']['Table'];
        $columns = $this->basicBrowserConfig['Browse']['Columns'];

        // NOTE: &$sort_info explicit set to reference

        $sort_orders = array(
            // current => next, rotating sort
            ''           => 'ascending',
            'ascending'  => 'descending',
            'descending' => ''
        );

        // READ CURRENT SORT VALUES AND SET
        if (isset($_POST['sort_order']) AND is_array($_POST['sort_order'])) {

            $sort_order_tainted = $_POST['sort_order'];
            $col_dummy_a        = array_keys($columns);
            $sort_values        = array_keys($sort_orders);

            foreach ($sort_order_tainted as $_t_col_dummy => $_t_col_value) {
                if (! in_array($_t_col_dummy, $col_dummy_a)) {
                    continue;
                }
                // $_t_col_name is now untainted
                $ok_col_dummy = $_t_col_dummy;
                if (! in_array($_t_col_value, $sort_values)) {
                    continue;
                }
                // $_t_col_value is now untainted
                $ok_col_value = $_t_col_value;

                if ($ok_col_value) {
                    $sort_info[$ok_col_dummy] = $ok_col_value;
                }
            }
        }

        if (! isset($_POST['BROWSE_ACTION'])) {
            return;
        }

        if (! is_array($_POST['BROWSE_ACTION'])) {
            return;
        }

        if (count($_POST['BROWSE_ACTION']) > 1) {
            return;
        }

        $action_tainted = array_keys($_POST['BROWSE_ACTION']);
        $action_tainted = $action_tainted[0];

        if ($action_tainted != 'SORT') {
            return;
        }

        // get the sort action, what field has been pressed

        if (! is_array($_POST['BROWSE_ACTION']['SORT'])) {
            return;
        }

        if (count($_POST['BROWSE_ACTION']['SORT']) > 1) {
            return;
        }

        $tainted_sort_field = array_keys($_POST['BROWSE_ACTION']['SORT']);
        $tainted_sort_field = $tainted_sort_field[0];

        if (! $tainted_sort_field) {
            return;
        }

        // $tainted_sort_field must be one of the displayed columns

        $ok = false;
        foreach ($columns as $col_dummy => $col_def) {
            if ($tainted_sort_field === $col_dummy) {
                $ok = true;
                break;
            }
        }

        if ($ok === false) {
            return;
        }

        // confirmed to be OK
        $sort_field = $tainted_sort_field;

        if (! isset($_POST['sort_order'])) {
            return;
        }

        if (! isset($_POST['sort_order'][$sort_field])) {
            return;
        }

        ////////////////////////////////////////////////////

        if (! in_array($_POST['sort_order'][$sort_field], $sort_orders)) {
            return;
        }

        $next_sort = $sort_orders[$_POST['sort_order'][$sort_field]];

        $sort_info = array(); // force only 1 sort at a time
        // later we may enable sort numbers to sort by multiple fields
        $sort_info[$sort_field] = $next_sort;
    }

    function process_page_amount($amount)
    {
        $x_amount = $this->basicBrowserConfig['Browse']['Amount'];

        if (! isset($_POST['page_amount'])) {
            if ($amount) {
                return $amount;
            }

            return $x_amount;
        }

        $page_amount_tainted = trim($_POST['page_amount']) * 1;

        if ($page_amount_tainted > 10000 OR $page_amount_tainted < 10) {
            return $amount;
        }

        $page_amount_ok = $page_amount_tainted;

        return $page_amount_ok;
    }

    private function retrieveRemember()
    {
        $x_search = null;
        $x_where  = null;
        $x_sort   = null;
        $x_amount = null;
        $x_offset = null;

        $here = current_url();
        if (isset($_SESSION['remember'][$here])) {

            $remember = $_SESSION['remember'][$here];

            if (isset($remember['where'])) {
                $x_where = $remember['where'];
            }
            if (isset($remember['search'])) {
                $x_search = $remember['search'];
            }
            if (isset($remember['sort'])) {
                $x_sort = $remember['sort'];
            }
            if (isset($remember['amount'])) {
                $x_amount = $remember['amount'];
            }
            if (isset($remember['offset'])) {
                $x_offset = $remember['offset'];
            }
        }

        return array($x_search, $x_where, $x_sort, $x_amount, $x_offset);
    }

    private function saveRemember($search_info, $where_info, $sort_info, $amount, $offset)
    {
        $remember                    = array(
            'where'  => $where_info,
            'search' => $search_info,
            'amount' => $amount,
            'sort'   => $sort_info,
            'offset' => $offset,
        );
        $here                        = current_url();
        $_SESSION['remember'][$here] = $remember;
    }

    function basicBrowser()
    {
        // This is the glue between the Renderer and the DataFetcher
        $sort_info   = null;
        $search_info = null;
        $offset      = 0;

        ///////////////////////////////////////////////////////
        // Handle Params from Interactions

        $Class  = $this->basicBrowserConfig['Browse']['Class'];
        $Alias  = $this->basicBrowserConfig['Browse']['Alias'];
        $table  = $this->basicBrowserConfig['Browse']['Table'];
        $tables = null;
        if (isset($this->basicBrowserConfig['Browse']['Tables'])) {
            $tables = $this->basicBrowserConfig['Browse']['Tables'];
        }

        $where_info  = null;
        $search_info = null;
        $sort_info   = null;
        $amount      = null;
        $offset      = null;

        list($search_info, $where_info, $sort_info, $amount, $offset) = $this->retrieveRemember();

        $amount                                       = $this->process_page_amount($amount);
        $this->basicBrowserConfig['Browse']['Amount'] = $amount;

        $this->process_where_fields(&$search_info, &$where_info);
        $this->process_sort_fields(&$sort_info);

        // the next line may be slow due to where clause, if no proper index
        $total                                       = $this->basicDataProcessor->get_query_count($this->basicBrowserConfig, $where_info);
        $this->basicBrowserConfig['Browse']['Total'] = $total;

        $offset                                       = $this->get_new_offset($offset);
        $this->basicBrowserConfig['Browse']['Offset'] = $offset;

        $this->saveRemember($search_info, $where_info, $sort_info, $amount, $offset);

        ////////////////////////////////////////////////////////
        // Get the data and display

        $data = $this->basicDataProcessor->get_basic_data($this->basicBrowserConfig, $where_info, $sort_info);

        $html = $this->render_as_2d_table($sort_info, $search_info, $data);

        return $html;
    }

}
