<?php

require_once("config/app.php");
require_once("config/database.php");
require_once("config/server.php");
require_once("core/routes.php");
require_once("model/Mainmodel.php");
require_once("controller/Maincontroller.php");

if(isset($_GET["c"])){
    $obj=cargar_controlador($_GET["c"]);
    if(isset($_GET["a"])){
        if(isset($_GET["id"])){
            cargar_accion($obj,$_GET["a"],$_GET["id"]);
        }else{
            cargar_accion($obj,$_GET["a"]);
        }
    }else{
        $a=accion_principal;
        cargar_accion($obj,$a);
    }
}else{
    $c=controlador_principal;
    $obj=cargar_controlador(controlador_principal);
    $a=accion_principal;
    $obj->$a();

}