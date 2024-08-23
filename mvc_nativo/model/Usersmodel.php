<?php

class Usersmodel extends Mainmodel{

    public function getUsersbyAdmin($id){
        return parent::selectbykey("Usuarios","ID_administrador",$id);
    }

    public function getUserbyid($id){
        return parent::selectbykey("Usuarios","ID_usuario",$id);

    }

    public function getUserbyname($name){
        $query="SELECT * FROM Usuarios WHERE nombre_usuario = '$name'";
        $r=$this->db->query($query);
        $result=$r->fetch_assoc();
        return $result;
    }

    public function getUserbyemail($mail){
        $query="SELECT * FROM Usuarios WHERE email='$mail'";
        $r=$this->db->query($query);
        $result=$r->fetch_assoc();
        return $result;
    }
    public function insertUser($name,$email,$pw,$id_p,$id_a=null){
        $hash=password_hash($pw,PASSWORD_DEFAULT);
        $data=[
            "nombre_usuario" => $name,
            "email" => $email,
            "hash_contrasena" => $hash,
            "fecha_creacion" => "CURRENT_TIMESTAMP",
            "ID_permiso" => $id_p

        ];

        if($id_a!==null){
            $data["ID_administrador"]=$id_a;

        }

        parent::insert("Usuarios",$data);
    }

    public function deleteUser($id){
        $query="DELETE FROM Usuarios WHERE ID_usuario=$id";
        $this->db->query($query);
    }

   
}