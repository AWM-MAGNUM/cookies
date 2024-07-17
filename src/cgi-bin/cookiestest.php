#!/usr/bin/php-cgi
<?php
// Définir un cookie
setcookie("TestCookie", "ValeurDuCookie", time() + 3600, "/"); // expire dans 1 heure

// Envoyer les en-têtes HTTP
header('Content-Type: text/html; charset=utf-8');

echo '<!DOCTYPE html>';
echo '<html lang="fr">';
echo '<head>';
echo '<meta charset="UTF-8">';
echo '<title>Test des Cookies</title>';
echo '</head>';
echo '<body>';

// Afficher les en-têtes de requête pour le débogage
echo "<h1>En-têtes de requête</h1>";
foreach (getallheaders() as $name => $value) {
    echo "$name: $value<br>";
}

// Afficher les cookies reçus du client
echo "<h1>Cookies reçus</h1>";
if(count($_COOKIE) > 0) {
    foreach ($_COOKIE as $key => $value) {
        echo "Nom du cookie: " . htmlspecialchars($key) . "<br>";
        echo "Valeur du cookie: " . htmlspecialchars($value) . "<br>";
    }
} else {
    echo "Aucun cookie reçu.";
}

echo "<h1>Test de définition de cookie</h1>";
echo "Un cookie nommé 'TestCookie' avec la valeur 'ValeurDuCookie' a été défini et expirera dans 1 heure.<br>";

echo "Veuillez recharger la page pour voir si le cookie est envoyé.";

echo '</body>';
echo '</html>';
?>
