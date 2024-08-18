<?php

class Database{
    public static function conectar(){
        $conexion = mysqli_connect(Server_db,Username,Password,db);
        return $conexion;
    }
}