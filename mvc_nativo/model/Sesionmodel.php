<?php

class Sesionmodel extends Mainmodel{
    public function createSesion($user,$token){
        $data=[
            "ID_usuario" =>$user,
            "token_sesion"=>$token,
            "fecha_inicio"=>"CURRENT_TIMESTAMP",
            "fecha_expiracion"=>"null",
            "estado"=>1
        ];

        parent::insert("sesiones",$data);
    }

    public function validateSesion($token){
       return parent::selectbykey("sesiones","token_sesion",$token);
    }
}