<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
</head>
<body>
<h1><center>Registro de prueba</center></h1>

<?php if($error){
    echo $error;
}
?>
    <form>
        <input type="text" placeholder="Nombre de usuario" required name="username">
        <label for="username">Ingrese usuario</label><br>

        <input type="text" placeholder="E-mail" required name="email">
        <label for="email">Ingrese su e-mail</label><br>

        <input type="password" placeholder="Contraseña" required name="password">
        <label for="password">Ingrese contraseña</label><br>

        <input type="checkbox" name="remember">
        <label for="remember">Recordarme</label><br>

        <input type="submit" value="Ingresar">
    </form>
</body>
</html>