#!/bin/bash
# usage: auto-index [dir]
INDEX=`find $1 -maxdepth 1 -type d \( ! -iname "." ! -iname "CMakeFiles" \) | sed "s/^.*/      <li\>\<a\ href=\"&\\/index.html\"\>&\<\\/a\>\<\\/li\>/"`
echo "<html>
<head><title>Razor-qt documentation index</title></head>
<body>
<h1>Razor-qt Documentation Index</h1>
<ui>
$INDEX
<ui>
</body>
</html>"

