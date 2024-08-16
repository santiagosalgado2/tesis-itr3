<?php

class Modelo{
    private $db;
    private $datos;
    public function __construct(){
        $this->db=Database::conectar();
        $this->datos=array();
    }

    public function getUsersbyAdmin($id){
        $query="SELECT * FROM Usuarios WHERE ID_administrador = $id";
    }
}