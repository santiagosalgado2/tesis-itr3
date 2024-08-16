<?php

class Database{
    public static function conectar(){
        $conexion = mysqli_connect("localhost","root","","tesis_gs");
        return $conexion;
    }
}