#!/usr/bin/php-cgi
<!DOCTYPE html>
<html lang="en">
<?php
if ($_SERVER["REQUEST_METHOD"] == "POST") {
    if (isset($_POST["name"])) {
        $name = htmlspecialchars($_POST["name"]);
        echo "Success! Received name: " . $name;
    } else {
        echo "Error! Name variable not received.";
    }
} else {
    echo "Error! Method not allowed.";
}
?>
</html>
