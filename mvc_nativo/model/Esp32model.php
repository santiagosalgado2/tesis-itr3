<?php
class Esp32model extends Mainmodel{
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

    public function getEsp32($id){
        $query="SELECT * FROM esp32 WHERE ID_esp32=$id LIMIT 1";

        return parent::getRegistro($query);
    }

    public function insertEsp32($mac,$ip,$ubicacion,$estado){
        $data=[ 
            "direccion_mac"=> $mac,
            "direccion_ip"=> $ip,
            "ubicacion"=> $ubicacion,
            "estado"=> $estado
        ];
        parent::insert("esp32",$data);
    }

}