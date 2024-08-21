<?php
class Irsignalsmodel extends Mainmodel{
    public function insertIrsignal($hex,$disp,$func){
        $query="INSERT INTO senales_ir(`codigo_hexadecimal`, `ID_dispositivo`, `ID_funcion`) VALUES
        ($hex,$disp,$func)";
        $this->db->query($query);
    }

    public function deleteIrsignal($id){
        $query="DELETE FROM senales_ir WHERE ID_senal = $id";
        $this->db->query($query);
    }
}