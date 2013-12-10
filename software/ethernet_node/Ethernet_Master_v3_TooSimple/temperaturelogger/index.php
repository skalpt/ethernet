<?php
	$node = $_GET["node"];
	$data = $_GET["data"];
	if(!isset($node) | !isset($data)) die("Error: Invalid query string");
	
	// Connect to SQL server and select database
	$con = mysql_connect("localhost", "thenewd1", "Girlfriend0") or die("Error: " . mysql_error());
	$result = mysql_select_db("thenewd1_temperaturelogger", $con) or die("Error: " . mysql_error());
	$query =
	"
		INSERT INTO tbl_Data (Node, DataValue)
			VALUES
			(
				'$node',
				'$data'
			);
	";
	$result = mysql_query($query) or die("Error: " . mysql_error());

	echo "Success: data uploaded.";
?>