<?php

	if($_GET['v'] && strlen($_GET['v']) >= 3)
	{
		switch($_GET['v'])
		{
			case '2.0':
				require('2-0.php');
				break;
			case '2.1':
				require('2-1.php');
				break;
			default:
				require('2-0.php');
				break;
		}
	} else {
		require('2-0.php');
	}

?>