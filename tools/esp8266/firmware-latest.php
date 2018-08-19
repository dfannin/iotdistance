<?php

$db = array(
    "5C:CF:7F:0E:6A:5F" => "iotdistance.ino",
    "5C:CF:7F:0E:6A:5C" => "iotdistance.ino",
    "5C:CF:7F:0E:6A:66" => "iotdistance.ino",
);

header('Content-type: text/plain; charset=utf8',true) ;

function check_header($name,$value = false) {
    if(!isset($_SERVER[$name])) {
        return false ; 
    }
    if($value && $_SERVER[$name] != $value ) {
        return false ;
    }
    return true ; 
}

function elogger($msg) {

    $dts = strftime('%c') ; 
    file_put_contents('php://stderr', "[" . $dts . "] [firmware-latest] " . $msg . "\n")  ; 
}


function sendFile($path) {
	header($_SERVER["SERVER_PROTOCOL"].' 200 OK', true, 200);
    header('Content-Type: application/octet-stream', true);
    header('Content-Disposition: attachment; filename='.basename($path));
    header('Content-Length: '.filesize($path), true);
    header('x-MD5: '.md5_file($path), true);
    readfile($path);
}

/*
 * check for proper request variables
 */

if(!check_header('HTTP_USER_AGENT', 'ESP8266-http-Update')) {
    header($_SERVER["SERVER_PROTOCOL"].' 403 Forbidden', true, 403);
    echo "only for ESP8266 updater!\n";
    elogger("error: invalid esp8266 update request") ; 
    exit();
}

if(
    !check_header('HTTP_X_ESP8266_STA_MAC') ||
    !check_header('HTTP_X_ESP8266_AP_MAC') ||
    !check_header('HTTP_X_ESP8266_FREE_SPACE') ||
    !check_header('HTTP_X_ESP8266_SKETCH_SIZE') ||
    !check_header('HTTP_X_ESP8266_CHIP_SIZE') ||
    !check_header('HTTP_X_ESP8266_SDK_VERSION')
) {
    header($_SERVER["SERVER_PROTOCOL"].' 403 Forbidden', true, 403);
    echo "only for ESP8266 updater! (header)\n";
    elogger("error: esp8266 update header variables missing or invalid") ; 
    exit();
}


/*
 * if a fw version is based, save it
 * in this version, we don't need a version,
 * we will just load the latest one.
 */ 
if ( isset($_SERVER['HTTP_X_ESP8266_VERSION']) ) {
    $version_check = true ; 
    $fw_ver = $_SERVER['HTTP_X_ESP8266_VERSION'] ; 
    elogger("status: device updating from " . $fw_ver); 
} else {
    $version_check = False ; 
    $fv_ver = "" ; 
} 

/*
 * check if the iot device mac addr is in the db
 */ 

$mac = strtoupper( $_SERVER['HTTP_X_ESP8266_STA_MAC'] ) ; 

if( isset($db[$mac])) {
    $fw_base = $db[$mac] ;
    $fw_file = "./" . $fw_base . ".bin"  ; 
} else {
   header($_SERVER["SERVER_PROTOCOL"].' 500 not in iot database', true, 500);
   elogger("error:  device not in iot database") ; 
   exit() ;
}


if ( file_exists( $fw_file ) ) {
   sendFile($fw_file);
} else {
    header($_SERVER["SERVER_PROTOCOL"].' 500 no version for ESP MAC', true, 500);
    elogger("error: firmware file does not exist on server"); 
    exit(); 
}

//  header($_SERVER["SERVER_PROTOCOL"].' 304 Not Modified', true, 304);
//  header($_SERVER["SERVER_PROTOCOL"].' 500 no version for ESP MAC', true, 500);

?>
