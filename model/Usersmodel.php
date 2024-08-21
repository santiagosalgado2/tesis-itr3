<?php

class Usersmodel extends Mainmodel{

    public function getUsersbyAdmin($id){
        return parent::selectbykey("Usuarios","ID_administrador",$id);
    }

    public function getUser($id){
        return parent::selectbykey("Usuarios","ID_usuario",$id);

    }
    public function insertUser($name,$email,$pw,$salt,$fecha,$id_p,$id_a=null){
        $data=[
            "nombre_usuario" => $name,
            "email" => $email,
            "hash_contrasena" => $pw,
            "salt" => $salt,
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

    echo "";
}