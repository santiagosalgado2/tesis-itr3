<?php

    $caracteres_permitidos = '0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ';
    $longitud = 8;
    echo substr(str_shuffle($caracteres_permitidos), 0, $longitud);
    
  