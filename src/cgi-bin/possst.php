#!/usr/bin/php-cgi
<?php
header('Content-Type: text/html');
session_start();

// Debugging: Log file to check if the script is run multiple times
file_put_contents('debug.log', "Script run at: " . date('Y-m-d H:i:s') . "\n", FILE_APPEND);

if ($_SERVER['REQUEST_METHOD'] == 'POST') {
    // Check if the form has already been submitted in the current session
    if (isset($_SESSION['form_submitted'])) {
        file_put_contents('debug.log', "Form already submitted.\n", FILE_APPEND);
        echo 'Form already submitted.';
        exit;
    }

    // Log request headers for additional context
    foreach (getallheaders() as $name => $value) {
        file_put_contents('debug.log', "$name: $value\n", FILE_APPEND);
    }

    // Log POST data
    foreach ($_POST as $key => $value) {
        file_put_contents('debug.log', "POST $key: $value\n", FILE_APPEND);
    }

    // Log uploaded files
    foreach ($_FILES as $file) {
        file_put_contents('debug.log', "File uploaded: " . $file['name'] . "\n", FILE_APPEND);
    }

    // save $_FILES['avatar'] to a inside a folder
    $avatar = $_FILES['avatar'];
    $avatar_name = $avatar['name'];
    $avatar_tmp_name = $avatar['tmp_name'];
    $avatar_size = $avatar['size'];
    $avatar_error = $avatar['error'];

    file_put_contents('debug.log', "POST request received with file: " . $avatar_name . "\n", FILE_APPEND);

    $avatar_ext = explode('.', $avatar_name);
    $avatar_actual_ext = strtolower(end($avatar_ext));

    $allowed = array('jpg', 'jpeg', 'png');

    if (in_array($avatar_actual_ext, $allowed)) {
        if ($avatar_error === 0) {
            if ($avatar_size < 1000000) {
                $avatar_name_new = uniqid('', true) . '.' . $avatar_actual_ext;
                $avatar_destination = './' . $avatar_name_new;
                move_uploaded_file($avatar_tmp_name, $avatar_destination);

                // Log upload success
                file_put_contents('debug.log', "File uploaded to: " . $avatar_destination . "\n", FILE_APPEND);

                $_COOKIE['name'] = $_POST['name'];
                $_COOKIE['email'] = $_POST['email'];
                $_COOKIE['avatar'] = $avatar_destination;

                // Set cookie expiration time based on "Remember me" checkbox
                $cookie_expiration = isset($_POST['remember_me']) ? time() + 3600 * 24 * 7 : time() + 3600 * 24;

                setcookie('name', $_POST['name'], $cookie_expiration);
                setcookie('email', $_POST['email'], $cookie_expiration);
                setcookie('avatar', $avatar_destination, $cookie_expiration);
                echo isset($_COOKIE['name']);

                // Set the session variable to indicate the form has been submitted
                $_SESSION['form_submitted'] = true;
            } else {
                file_put_contents('debug.log', "File too big\n", FILE_APPEND);
                echo 'File too big';
                exit(1);
            }
        } else {
            file_put_contents('debug.log', "Error uploading file\n", FILE_APPEND);
            echo 'Error';
            exit(1);
        }
    } else {
        file_put_contents('debug.log', "File type not allowed\n", FILE_APPEND);
        echo 'Not allowed';
        exit(1);
    }
} else if ($_SERVER['REQUEST_METHOD'] == 'GET' && isset($_GET['logout'])) {
    unset($_COOKIE['name']);
    unset($_COOKIE['email']);
    unset($_COOKIE['avatar']);
    setcookie('name', '', time() - 3600);
    setcookie('email', '', time() - 3600);
    setcookie('avatar', '', time() - 3600);

    // Unset the session variable on logout
    unset($_SESSION['form_submitted']);

    file_put_contents('debug.log', "User logged out\n", FILE_APPEND);
}
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Form Submission</title>
    <script>
    function preventMultipleSubmissions(form) {
        form.querySelector('input[type="submit"]').disabled = true;
        return true;
    }
    </script>
</head>
<body>
<div>
    <?php if (isset($_COOKIE['name']) && isset($_COOKIE['email'])): ?>
        <p>
            <center><h1>Hello, <?= htmlspecialchars($_COOKIE['name']) ?>!</h1></center><hr>
            <center><img src="<?= htmlspecialchars($_COOKIE['avatar']) ?>" alt="avatar" width="300" height="300" style="object-fit: cover;"></center><hr><br/>
            <center><?= htmlspecialchars($_COOKIE['name']); ?></center>
            <center><?= htmlspecialchars($_COOKIE['email']); ?></center><br/><br/>
            <center><a href="?logout">Logout</a></center>
        </p>
    <?php else: ?>
        <form method="post" enctype="multipart/form-data" onsubmit="preventMultipleSubmissions(this);">
            <input type="text" name="name" placeholder="Name" required />
            <input type="text" name="email" placeholder="Email" required />
            <input type="file" id="avatar" name="avatar" accept="image/png, image/jpeg" required />
            <label for="remember_me">Remember me:</label>
            <input type="checkbox" name="remember_me" id="remember_me" value="1" />
            <input type="submit" value="Submit" />
        </form>
    <?php endif; ?>
</div>
</body>
</html>
