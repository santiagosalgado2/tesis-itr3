<?php
class Mainmodel{
    protected $db;
    protected $datos;
    protected $registro;
    public function __construct(){
        $this->db=Database::conectar();
        $this->datos=array();
    }

    public function getDatos($stmt){
        while ($row=$stmt->fetch_assoc()){
            $this->datos[]=$row;
        }
        return $this->datos;
    }

    public function getRegistro($stmt){
       
        $this->registro=$stmt->fetch_assoc();
        return $this->registro;
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

    public function selectbykey($table,$k,$id){
        $table = preg_replace('/[^a-zA-Z0-9_]/', '', $table);
        $k = preg_replace('/[^a-zA-Z0-9_]/', '', $k);
        $query="SELECT * FROM $table WHERE $k=:id";

        $stmt=$this->db->prepare($query);

        $stmt->execute(["id"=>$id]);

        return $this->getRegistro($stmt);
    }
}