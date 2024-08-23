<?php

class Sesioncontroller extends Maincontroller{
    private $users;
    private $sesion;

    public function __construct(){
        require_once("model/Usersmodel.php");
        require_once("model/Sesionmodel.php");
        $this->users=new Usersmodel;
        $this->sesion=new Sesionmodel;
    }
    public function register_view(){
        require_once("view/register.php");
    }

    public function register(){
        if ($_SERVER['REQUEST_METHOD'] === 'POST') {
            $username = $_POST['username'];
            $password = $_POST['password'];
            $email = $_POST['email'];

            $verify_name=$this->users->getUserbyname($username);
            $verify_mail=$this->users->getUserbyemail($email);

            if($verify_name OR $verify_mail){
                $error="El nombre de usuario ya existe o el Mail ya esta registrado";
                $this->register_view();
            }else{
                $this->users->insertUser($username,$email,$password,1);
                $userID=$this->users->getUserbyname($username);
                $token = bin2hex(random_bytes(32));
                $this->sesion->createSesion($userID["ID_usuario"],$token);

            }

            session_start();

            $_SESSION["user_id"]=$userID["ID_usuario"];
            $_SESSION["token"]=$token;

            header("Location: http://localhost/tesis_g_s/tesis-itr3/mvc_nativo/");
            exit;
        }
    }


}