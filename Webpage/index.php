<?php
        include ('dbcon.php');

        $stmt = $con->prepare('SELECT * FROM state ORDER BY datetime DESC');
        $stmt -> execute();

        $stmt -> setFetchMode(PDO::FETCH_ASSOC);

	$i=0;
	while($row = $stmt->fetch()){
		$ds_time[$i]=$row['datetime'];
		$ds_servo[$i]=$row['ServoAngle'];
		$ds_velo[$i]=$row['Velocity'];
		$ds_trans[$i]=$row['Transmission'];
		$ds_battery[$i]=$row['BatteryVoltage'];
		$ds_state[$i]=$row['CarState'];
		$i++;
	}
?>


<!DOCTYPE html>
<html lang="ko">
	<head>
		<meta http-equiv="Content-Type"  content="text/html; charset=UTF-8" /> 
        <title>Double Stone</title>
        <style>
	@import url('https://fonts.googleapis.com/css2?family=Nanum+Gothic&display=swap');

	.jm-font{
		font-family: 'Nanum Gothic', sans-serif;
		color: black;
	}
	body{
		font-family: 'Nanum Gothic', sans-serif;
		font-family: 12px;
	}
	table{
		width: 70%;
	}
	th, td{
		padding: 10px;
		border-bottom: 1px solid #dadada;
		text-align: center;
	}
        </style>
    </head>
	<body>

	<div align="center">
	<h3>DOUBLE STONE</h3>
        <table>
	<thead>
		<tr bgcolor=#b2ccff>
		<th>Time</th>
		<th>Servo Angle</th>
		<th>Velocity</th>
		<th>Transmission</th>
		<th>Battery Voltage</th>
		<th>Car State</th>
		</tr>
	<thead>
	<tbody>
<?php
	for($i=0;$i<5;$i++){
		echo "<tr><td>". $ds_time[$i]."</td><td>".$ds_servo[$i]."</td><td>".$ds_velo[$i]."</td><td>".$ds_trans[$i]."</td><td>".$ds_battery[$i]."</td><td>".$ds_state[$i]."</td></tr>";
	}
?>

	</tbody>
        </table>
	<br/>
	<a href="showall.php">더보기</a>
    </body>
</html>
