<?php
////////////////////////////////////////////////////////
// The Doctrine Query Wrapper
require_once dirname(__FILE__) . '/' . "basicDataProcessor.inc";

////////////////////////////////////////////////////////
// The browser Render object that will handle all interactions with the user
require_once dirname(__FILE__) . '/' . "basicBrowserRender.inc";

///////////////////////////////////////////////////
///////////////////////////////////////////////////

Class VodProviderBrowserApp extends basicBrowserRender 
{

	// TODO: Rework this into Yaml

	protected $basicBrowserConfig = array(
		'Defaults' => array(
			'Browse' => array(
				'Amount' => 25,
			),
		),
		'Browse' => array(
			'Title' => '',
	
			'Amount' => 25,
			'Offset' => 0,
			'Total' => 0,
	
			'Table' => 'vodProvider',
			'Class' => 'VodProvider',
			'Alias' => 'vp',
		
			'Columns' => array(
				// Note the cols will be shown i this sequence
				'vodproviderid' => array(
					'showLabel' => 'Id',
					'sortable' => FALSE,
					'searchable' => FALSE,
					'colName' => 'vodproviderid',
					'visible' => FALSE,
				),
				// Add hidden columns
				'vodprovideruniquecode' => array(
					'showLabel' => 'Code',
					'sortable' => TRUE,
					'searchable' => TRUE,
					'colname' => 'vodprovideruniquecode',
					'visible' => TRUE,
					// The code will be the connector to the view/edit applet
					'islink' => array(
						'callback' => array(
							'function' => 'mk_link_vodProviderId',
							'params' => array(
								'vodProviderId',
							),
						),
					),
				),
				'vodprovidername' => array(
					'showLabel' => 'Name',
					'sortable' => TRUE,
					'searchable' => TRUE,
					'colName' => 'vodprovidername',
					'visible' => TRUE,
				),
				'vodProviderDescription' => array(
					'showLabel' => 'Description',
					'sortable' => FALSE,
					'searchable' => FALSE,
					'colName' => 'vodProviderDescription',
					'visible' => TRUE,
				),
				'vodProviderAttributes' => array(
					'showLabel' => 'Attributes',
					'sortable' => FALSE,
					'searchable' => FALSE,
					'colName' => 'vodProviderAttributes',
					'visible' => TRUE,
				),
				'movieMaxValidTimeInMonths' => array(
					'showLabel' => 'MovieExp',
					'sortable' => FALSE,
					'searchable' => FALSE,
					'colName' => 'movieMaxValidTimeInMonths',
					'visible' => TRUE,
				),
				'canHaveZeroPrices' => array(
					'showLabel' => 'ZeroPrice',
					'sortable' => FALSE,
					'searchable' => FALSE,
					'colName' => 'canHaveZeroPrices',
					'visible' => TRUE,
				),
			),
		),
	);

	public function __construct($conn)
	{
		parent::__construct($conn);
	}

	public function mk_link_vodProviderId($params) 
	{
		if(0) {
			print "<pre>"; 
			print_r($params); 
			print "</pre>";
		}
	
		$x1 = $params['__ROW__']['vodproviderid'];
		$x2 = $params['__ROW__']['vodprovideruniquecode'];
		$href = sprintf("/vproviders/edit/%s", $x1 );

		$data = <<<EOS
		<a href="$href" class="linkData" > $x2 </a>
EOS;
		return $data;
	}
}

function vodProviderBrowserApp() 
{
	Doctrine_Manager::getInstance()->bindComponent('VodProvider', 'bnet');
	$conn = Doctrine_Manager::connection();
	//////////////////////////
	// Setup the Browser Applet
	$VodProviderBrowserApp = New VodProviderBrowserApp($conn);

	// Interact with the user
	$html = $VodProviderBrowserApp -> basicBrowser();

	return $html;
}

