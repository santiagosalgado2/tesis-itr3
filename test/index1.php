<?php

class prueba{

private $db;

public function __construct(){
    $this->db=mysqli_connect("localhost","root","","test");

}
public function insert($table,$data){
    $columns= implode(", ",array_keys($data));

    $placeholders = [];
    $types = '';
    $values = [];

    foreach ($data as $key => $value) {
        if ($value === 'CURRENT_TIMESTAMP') {
            $placeholders[] = 'CURRENT_TIMESTAMP';  // Insertar directamente en la consulta
        } else {
            $placeholders[] = '?';
            $types .= is_int($value) ? 'i' : 's';  // 'i' para enteros, 's' para strings
            $values[] = $value;
        }
    }

    $placeholders = implode(", ", $placeholders);

    // Preparar la consulta SQL
    $query = "INSERT INTO $table ($columns) VALUES ($placeholders)";
    $stmt = $this->db->prepare($query);

    if ($values) {
        $stmt->bind_param($types, ...$values);  // Asociar los valores con los marcadores de posición
    }

    $stmt->execute();  // Ejecutar la consulta
    

}
}
$data=[
    "nombre_usuario" => "user",
    "email" => "asd@gmail.com",
    "hash_contrasena" => "hash123",
    "salt" => "salt123",
    "fecha_creacion" => "CURRENT_TIMESTAMP",
    "ID_permiso" => 1

];

$obj=new prueba;
$obj->insert("Usuarios",$data);