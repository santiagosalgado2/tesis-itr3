<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
</head>
<body>
<?php if(isset($error)){
    echo $error;
}
?>
    <h1><center>Login de prueba</center></h1>
    <form method="post" action="index.php?c=Sesion&a=login">
        <input type="text" placeholder="Nombre de usuario" required name="username">
        <label for="username">Ingrese usuario</label><br>

        <input type="password" placeholder="Contraseña" required name="password">
        <label for="password">Ingrese contraseña</label><br>

        <input type="checkbox" name="remember">
        <label for="remember">Recordarme</label><br>

        <input type="submit" value="Ingresar">
    </form>
<br><br>
    <a href="index.php?c=Sesion&a=register_view">¿No tienes una cuenta? Registrate</a>
</body>
</html>