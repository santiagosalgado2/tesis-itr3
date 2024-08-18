<?php
class Mainmodel{
    protected $db;
    protected $datos;
    protected $registro;
    public function __construct(){
        $this->db=Database::conectar();
        $this->datos=array();
    }

    public function getDatos($query){
        $r=$this->db->query($query);
        while ($row=$r->fetch_assoc()){
            $this->datos[]=$row;
        }
        return $this->datos;
    }

    public function getRegistro($query){
        $result=$this->db->query($query);
        $this->registro=$result->fetch_assoc();
        return $this->registro;
    }
}