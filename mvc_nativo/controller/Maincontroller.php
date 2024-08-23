<?php

class Maincontroller{

    public function index(){
        if(isset($_SESSION["username"]) && isset($_SESSION["token"])){
            require_once("view/inicio.php");
        }else{
            require_once("view/login.php");
        }
        
    }
}