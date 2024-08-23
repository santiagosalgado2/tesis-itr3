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


            if($this->users->getUserbyname($username) OR $this->users->getUserbyemail($email)){
                $error="El nombre de usuario ya existe o el Mail ya esta registrado";
                $this->register_view();
            }else{
                $this->users->insertUser($username,$email,$password,1);
                $userID=$this->users->getUserbyname($username);
                $token = bin2hex(random_bytes(32));
                $this->sesion->createSesion($userID["ID_usuario"],$token);

            }

            if(isset($_SESSION["username"])){
                session_unset();  // Limpia todas las variables de sesión
                session_destroy(); // Destruye la sesión
                
            }
            session_start();
                $_SESSION["user_id"]=$userID["ID_usuario"];
                $_SESSION["token"]=$token;
                $_SESSION["username"]=$username;
            require_once("view/inicio.php");
        }
    }

    public function login(){
        

        if ($_SERVER['REQUEST_METHOD'] === 'POST') { 
            $username = $_POST['username'];
            $password = $_POST['password'];

            $user=$this->users->getUserbyname($username);

            if($user && password_verify($password,$user["hash_contrasena"])){
                if(isset($_SESSION["username"])){
                    session_unset();  // Limpia todas las variables de sesión
                    session_destroy();
                    
                }else{
                    $_SESSION["username"]=$username;
                    $_SESSION["password"]=$password;
                    $token = bin2hex(random_bytes(32));
                    $this->sesion->createSesion($user["ID_usuario"],$token);
                }

                require_once("view/inicio.php");
            }else{
                $error="Usuario o contraseña incorrectos";
                require_once("view/login.php");
            }
        }else{
            require_once("view/login.php");
        }
    }


}