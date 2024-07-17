#!/usr/bin/php-cgi
<?php
if (isset($_POST["sub"])) {
    if (isset($_POST["name"])) {
        $cookie_name = "user";
        $cookie_value = $_POST["name"];
        setcookie($cookie_name, $cookie_value, time() + 3600);
        echo "Cookie has been set with the value: " . $cookie_value;
    } else {
        echo "Please enter a name before submitting.";
    }
}
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
</head>
<body> 
    <form  method="POST" action="">
        <input name="name" />
        <input name="sub" type="submit" />
    </form>
</body>
</html>