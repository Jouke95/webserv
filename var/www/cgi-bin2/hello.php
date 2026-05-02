#!/usr/bin/php-cgi
<?php
$method = getenv("REQUEST_METHOD") ?: "";
$query  = getenv("QUERY_STRING") ?: "";

$params = [];
parse_str($query, $params);

$name = $params["name"] ?? "";
$age  = $params["age"]  ?? "";

function row($label, $value) {
    $val = $value !== "" ? htmlspecialchars($value) : "<em>empty</em>";
    return "<div class='row'><span class='label'>{$label}</span><span class='value'>{$val}</span></div>";
}

$output = <<<HTML
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title>hello.php — webserv</title>
<style>
  @import url('https://fonts.googleapis.com/css2?family=JetBrains+Mono:wght@400;600&display=swap');
  :root {
    --bg:#0a0a0f;--surface:#111118;--border:#1e1e2e;
    --accent:#7b8cff;--text:#e0e0f0;--muted:#555570;
    --mono:'JetBrains Mono',monospace;
  }
  *{margin:0;padding:0;box-sizing:border-box;}
  body{background:var(--bg);color:var(--text);font-family:var(--mono);min-height:100vh;padding:48px 24px;}
  body::before{content:'';position:fixed;inset:0;
    background-image:linear-gradient(rgba(123,140,255,0.03) 1px,transparent 1px),linear-gradient(90deg,rgba(123,140,255,0.03) 1px,transparent 1px);
    background-size:40px 40px;pointer-events:none;}
  .wrap{max-width:600px;margin:0 auto;position:relative;z-index:1;}
  .back{font-size:11px;color:var(--muted);text-decoration:none;display:block;margin-bottom:32px;}
  .back:hover{color:var(--accent);}
  .badge{font-size:11px;color:var(--accent);letter-spacing:3px;text-transform:uppercase;margin-bottom:12px;display:flex;align-items:center;gap:8px;}
  .badge::before{content:'';width:6px;height:6px;background:var(--accent);border-radius:50%;box-shadow:0 0 8px var(--accent);}
  h1{font-size:22px;font-weight:600;margin-bottom:32px;}
  h1 span{color:var(--accent);}
  .row{display:flex;gap:16px;padding:12px 16px;border-bottom:1px solid var(--border);font-size:13px;}
  .row:first-child{border-top:1px solid var(--border);}
  .label{color:var(--muted);width:140px;flex-shrink:0;}
  .value{color:var(--text);}
  em{color:#333;font-style:normal;}
  .hint{font-size:11px;color:var(--muted);margin-top:24px;}
  .hint a{color:var(--accent);text-decoration:none;}
</style>
</head>
<body>
<div class="wrap">
  <a class="back" href="/">← back to index</a>
  <div class="badge">🐘 php CGI</div>
  <h1>hello <span>world</span></h1>
HTML;

$output .= row("method", $method);
$output .= row("query_string", $query);
$output .= row("name", $name);
$output .= row("age", $age);

$output .= <<<HTML
  <p class="hint">try: <a href="/cgi/hello.php?name=jouke&age=21">/cgi/hello.php?name=jouke&age=21</a></p>
</div>
</body>
</html>
HTML;

header("Content-Type: text/html");
header("Content-Length: " . strlen($output));
echo $output;
?>