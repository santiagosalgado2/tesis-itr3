<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
</head>
<body>


    <h1>
        Pagina de inicio
    </h1>
    <h2>Bienvenido <?php echo $_SESSION["username"];?> </h2>

    <a href="index.php?c=Sesion&a=logout">Cerrar sesion</a>
</body>
</html>