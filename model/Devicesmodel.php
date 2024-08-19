<?php
class Devicesmodel extends Mainmodel{
    public function getDevicesbyEsp32($id){
        $query="SELECT
        d.nombre
        FROM dispositivos d 
        JOIN control_dispositivos cd ON cd.ID_dispositivo=d.ID_dispositivo
        JOIN esp32 e ON e.ID_esp32=cd.ID_esp32
        WHERE e.ID_esp32=$id";
        
        return parent::getDatos($query);
    }

    public function getDevicesbyUserandEsp32($id,$esp32){
        $query="SELECT
        d.nombre
        FROM dispositivos d 
        JOIN control_dispositivos cd ON cd.ID_dispositivo=d.ID_dispositivo
        JOIN esp32 e ON e.ID_esp32=cd.ID_esp32
        JOIN acceso_usuarios au ON au.ID_control=cd.ID_control
        JOIN usuarios u ON u.ID_usuario=au.ID_usuario
        WHERE e.ID_esp32=$esp32 AND u.ID_usuario=$id";

        return parent::getDatos($query);
    }

    public function getDevice($id){
        $query="SELECT * FROM Dispositivos WHERE ID_dispositivo=$id LIMIT 1";
        return parent::getRegistro($query);
    }

    public function insertDevice($nombre,$tipo){
        $query="INSERT INTO `dispositivos`(`nombre`, `ID_tipo`) VALUES 
        ($nombre,$tipo)";
        $this->db->query($query);
    }
}