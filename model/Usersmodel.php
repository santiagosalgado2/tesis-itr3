<?php

class Usersmodel extends Mainmodel{

    public function getUsersbyAdmin($id){
        $query="SELECT * FROM Usuarios WHERE ID_administrador = $id";
        return parent::getDatos($query);
    }

    public function getUser($id){
        $query="SELECT * FROM Usuarios WHERE ID_usuario=$id LIMIT 1";
        return parent::getRegistro($query);

    }

    public function getEsp32byUser($id){
        $query="SELECT 
        e.ubicacion
        FROM esp32 e 
        JOIN control_dispositivos cd ON cd.ID_esp32=e.ID_esp32
        JOIN acceso_usuarios au ON au.ID_control=cd.ID_control
        JOIN usuarios u ON u.ID_usuario=au.ID_usuario
        WHERE u.ID_usuario=$id
        GROUP BY e.ID_esp32";
        return parent::getDatos($query);
    }

    public function insertUser($name,$email,$pw,$salt,$fecha,$id_p,$id_a=null){
        if($id_a==null){
            $query="INSERT INTO Usuarios 
            (`nombre_usuario`, `email`, `hash_contrasena`, `salt`, `fecha_creacion`, `ID_permiso`, `ID_administrador`) 
            VALUES
            ($name,$email,$pw,$salt,$fecha,$id_p,NULL)";
        }else{
            $query="INSERT INTO Usuarios 
            (`nombre_usuario`, `email`, `hash_contrasena`, `salt`, `fecha_creacion`, `ID_permiso`, `ID_administrador`) 
             VALUES
            ($name,$email,$pw,$salt,$fecha,$id_p,$id_a)";
        }
        
        $this->db->query($query);
    }


    
}