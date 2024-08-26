<?php

class Maincontroller{

    public function index(){
        require_once("model/Sesionmodel.php");
        $obj=new Sesionmodel;
        session_start();
        $obj->createSesion($_SESSION["user_id"],$_SESSION["token"]);
        
        if(isset($_SESSION["username"]) && isset($_SESSION["token"])){
            require_once("view/inicio.php");
        }else{
            require_once("view/login.php");
        }
        
    }
}