<?php
$XX_CLI = TRUE;
// define constants used in hooks/doctrine.php, for instance
define ( APPPATH, dirname(__FILE__) );
define ( EXT, '.php');

// include and bootstrap
require_once 'hooks/doctrine.php';
$doctrine = bootstrap_doctrine();

// Configure Doctrine Cli
// Normally these are arguments to the cli tasks but if they are set here the arguments will be auto-filled

$config = array(
	'data_fixtures_path'=>  dirname(__FILE__) . DIRECTORY_SEPARATOR . '/fixtures',
	'models_path'		=>  dirname(__FILE__) . DIRECTORY_SEPARATOR . '/dmodels',
	'migrations_path'	=>  dirname(__FILE__) . DIRECTORY_SEPARATOR . '/migrations',
	'sql_path'			=>  dirname(__FILE__) . DIRECTORY_SEPARATOR . '/sql',
	'yaml_schema_path'	=>  dirname(__FILE__) . DIRECTORY_SEPARATOR . '/schema',
	'generateTableClasses'  =>  TRUE,
);

$cli = new Doctrine_Cli($config);
$cli->run($_SERVER['argv']);
