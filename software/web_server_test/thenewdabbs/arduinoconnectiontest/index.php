<?php

	// Get recipe ID from URL
	$data = $_GET["data"];
	
	if ($data)
	{
		// Connect to SQL server and select database
		$con = mysql_connect("localhost", "thenewd1", "Girlfriend0") or die("Error: " . mysql_error());
		$result = mysql_select_db("thenewd1_arduinoconnectiontest", $con) or die("Error: " . mysql_error());

		$query =
		"
			INSERT INTO tbl_Data (DataValue)
				VALUES
				(
					'$data'
				);
		";
		$result = mysql_query($query) or die("Error: " . mysql_error());
	}

?>