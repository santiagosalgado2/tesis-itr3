<?php

function cargar_controlador($controlador){
    $nombreC=ucwords($controlador)."controller";
    $ubicacionC="controller/".ucwords($controlador)."controller.php";

    if(!is_file($ubicacionC)){
        $ubicacionC="controller/".controlador_principal."controller.php";
    }



    require_once($ubicacionC);

    if(!class_exists($nombreC)){
        $nombreC=controlador_principal."controller";
    }
    $obj=new $nombreC;
    return $obj;
}

function cargar_accion($controlador,$accion,$id=null){
    if(method_exists($controlador,$accion)){
        if($id!=null){
            $controlador->$accion($id);
        }else{
            $controlador->$accion();
        }
    }else{
        $accion=accion_principal;
        $controlador->$accion();
    }
}