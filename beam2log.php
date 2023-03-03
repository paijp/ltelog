<?php
#
#	ltelog   https://github.com/paijp/ltelog
#
#	Copyright (c) 2023 paijp
#
#	This software is released under the MIT License.
#	http://opensource.org/licenses/mit-license.php
#

$a = json_decode(file_get_contents("php://input"), true);
file_put_contents("log.txt", date("Y-m-d H:i:s ").trim(base64_decode(@$a["payload"]))."\n", FILE_APPEND);
