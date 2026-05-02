<?php
$method = getenv("REQUEST_METHOD");
$query  = getenv("QUERY_STRING");

$params = [];
parse_str($query, $params);

$name = $params["name"] ?? "";
$age  = $params["age"] ?? "";
?>
Content-Type: text/html

<html>
<body>
<h1>Hallo van CGI!</h1>
<p>Method: <?= $method ?></p>
<p>Query string: <?= $query ?></p>
<p>Name: <?= $name ?></p>
<p>Age: <?= $age ?></p>
</body>
</html>