-- phpMyAdmin SQL Dump
-- version 5.2.1
-- https://www.phpmyadmin.net/
--
-- Servidor: 127.0.0.1
-- Tiempo de generación: 19-08-2024 a las 00:14:16
-- Versión del servidor: 10.4.32-MariaDB
-- Versión de PHP: 8.2.12

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Base de datos: `test`
--

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `acceso_usuarios`
--

CREATE TABLE `acceso_usuarios` (
  `ID_acceso` int(11) NOT NULL,
  `ID_usuario` int(11) NOT NULL,
  `ID_control` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;

--
-- Volcado de datos para la tabla `acceso_usuarios`
--

INSERT INTO `acceso_usuarios` (`ID_acceso`, `ID_usuario`, `ID_control`) VALUES
(1, 2, 1),
(2, 3, 2),
(3, 4, 3),
(4, 1, 1),
(5, 1, 2),
(6, 1, 3),
(7, 1, 4);

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `control_dispositivos`
--

CREATE TABLE `control_dispositivos` (
  `ID_control` int(11) NOT NULL,
  `ID_esp32` int(11) NOT NULL,
  `ID_dispositivo` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;

--
-- Volcado de datos para la tabla `control_dispositivos`
--

INSERT INTO `control_dispositivos` (`ID_control`, `ID_esp32`, `ID_dispositivo`) VALUES
(1, 1, 1),
(2, 2, 2),
(3, 3, 3),
(4, 2, 3);

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `dispositivos`
--

CREATE TABLE `dispositivos` (
  `ID_dispositivo` int(11) NOT NULL,
  `nombre` varchar(100) NOT NULL,
  `ID_tipo` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;

--
-- Volcado de datos para la tabla `dispositivos`
--

INSERT INTO `dispositivos` (`ID_dispositivo`, `nombre`, `ID_tipo`) VALUES
(1, 'TV Aula 101', 1),
(2, 'Proyector Aula 102', 2),
(3, 'AC Aula 101', 3);

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `esp32`
--

CREATE TABLE `esp32` (
  `ID_esp32` int(11) NOT NULL,
  `direccion_mac` varchar(30) NOT NULL,
  `direccion_ip` varchar(45) NOT NULL,
  `ubicacion` varchar(100) NOT NULL,
  `estado` tinyint(1) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;

--
-- Volcado de datos para la tabla `esp32`
--

INSERT INTO `esp32` (`ID_esp32`, `direccion_mac`, `direccion_ip`, `ubicacion`, `estado`) VALUES
(1, '00:1B:44:11:3A:B7', '192.168.1.10', 'Aula 101', 1),
(2, '00:1B:44:11:3A:B8', '192.168.1.11', 'Aula 102', 1),
(3, '00:1B:44:11:3A:B9', '192.168.1.12', 'Aula 103', 0);

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `funciones`
--

CREATE TABLE `funciones` (
  `ID_funcion` int(11) NOT NULL,
  `nombre` varchar(100) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;

--
-- Volcado de datos para la tabla `funciones`
--

INSERT INTO `funciones` (`ID_funcion`, `nombre`) VALUES
(1, 'Encender'),
(2, 'Apagar'),
(3, 'Cambiar canal'),
(4, 'Ajustar volumen');

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `login_attemps`
--

CREATE TABLE `login_attemps` (
  `ID_login_attemp` int(11) NOT NULL,
  `ID_usuario` int(11) DEFAULT NULL,
  `fecha` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  `exitoso` tinyint(1) NOT NULL,
  `direccion_ip` varchar(45) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;

--
-- Volcado de datos para la tabla `login_attemps`
--

INSERT INTO `login_attemps` (`ID_login_attemp`, `ID_usuario`, `fecha`, `exitoso`, `direccion_ip`) VALUES
(1, 1, '2024-08-18 13:01:00', 1, '192.168.1.100'),
(2, 2, '2024-08-18 14:01:00', 1, '192.168.1.101'),
(3, 3, '2024-08-18 14:31:00', 1, '192.168.1.102'),
(4, 4, '2024-08-18 15:01:00', 0, '192.168.1.103');

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `permisos`
--

CREATE TABLE `permisos` (
  `ID_permiso` int(11) NOT NULL,
  `nombre` varchar(30) NOT NULL,
  `descripcion` text NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;

--
-- Volcado de datos para la tabla `permisos`
--

INSERT INTO `permisos` (`ID_permiso`, `nombre`, `descripcion`) VALUES
(1, 'Administrador', 'Acceso completo a todos los recursos'),
(2, 'Profesor', 'Acceso limitado a dispositivos según curso'),
(3, 'Invitado', 'Acceso restringido, solo visualización');

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `senales_ir`
--

CREATE TABLE `senales_ir` (
  `ID_senal` int(11) NOT NULL,
  `codigo_hexadecimal` varchar(90) NOT NULL,
  `ID_dispositivo` int(11) NOT NULL,
  `ID_funcion` int(11) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;

--
-- Volcado de datos para la tabla `senales_ir`
--

INSERT INTO `senales_ir` (`ID_senal`, `codigo_hexadecimal`, `ID_dispositivo`, `ID_funcion`) VALUES
(1, 'A90B1C', 1, 1),
(2, 'A90B1D', 2, 2),
(3, 'A90B1E', 3, 3),
(4, 'A90B1F', 1, 4);

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `sesiones`
--

CREATE TABLE `sesiones` (
  `ID_sesion` int(11) NOT NULL,
  `ID_usuario` int(11) NOT NULL,
  `token_sesion` varchar(255) NOT NULL,
  `fecha_inicio` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  `fecha_expiracion` timestamp NULL DEFAULT NULL,
  `estado` tinyint(1) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;

--
-- Volcado de datos para la tabla `sesiones`
--

INSERT INTO `sesiones` (`ID_sesion`, `ID_usuario`, `token_sesion`, `fecha_inicio`, `fecha_expiracion`, `estado`) VALUES
(1, 1, 'token123', '2024-08-18 13:00:00', '2024-08-18 15:00:00', 1),
(2, 2, 'token456', '2024-08-18 14:00:00', '2024-08-18 16:00:00', 1),
(3, 3, 'token789', '2024-08-18 14:30:00', '2024-08-18 16:30:00', 1);

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `tipo_dispositivos`
--

CREATE TABLE `tipo_dispositivos` (
  `ID_tipo` int(11) NOT NULL,
  `nombre` varchar(100) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;

--
-- Volcado de datos para la tabla `tipo_dispositivos`
--

INSERT INTO `tipo_dispositivos` (`ID_tipo`, `nombre`) VALUES
(1, 'Televisor'),
(2, 'Proyector'),
(3, 'Aire acondicionado');

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `usuarios`
--

CREATE TABLE `usuarios` (
  `ID_usuario` int(11) NOT NULL,
  `nombre_usuario` varchar(40) NOT NULL,
  `email` varchar(100) NOT NULL,
  `hash_contrasena` varchar(128) NOT NULL,
  `salt` varchar(100) NOT NULL,
  `fecha_creacion` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  `ID_permiso` int(11) NOT NULL,
  `ID_administrador` int(11) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;

--
-- Volcado de datos para la tabla `usuarios`
--

INSERT INTO `usuarios` (`ID_usuario`, `nombre_usuario`, `email`, `hash_contrasena`, `salt`, `fecha_creacion`, `ID_permiso`, `ID_administrador`) VALUES
(1, 'admin1', 'admin1@example.com', '123hashed', 'salt1', '2024-08-18 13:00:00', 1, NULL),
(2, 'prof1', 'prof1@example.com', '456hashed', 'salt2', '2024-08-18 14:00:00', 2, 1),
(3, 'prof2', 'prof2@example.com', '789hashed', 'salt3', '2024-08-18 14:30:00', 2, 1),
(4, 'invitado1', 'invitado1@example.com', 'abc123hashed', 'salt4', '2024-08-18 15:00:00', 3, 1);

--
-- Índices para tablas volcadas
--

--
-- Indices de la tabla `acceso_usuarios`
--
ALTER TABLE `acceso_usuarios`
  ADD PRIMARY KEY (`ID_acceso`),
  ADD KEY `ID_usuario` (`ID_usuario`),
  ADD KEY `ID_control` (`ID_control`);

--
-- Indices de la tabla `control_dispositivos`
--
ALTER TABLE `control_dispositivos`
  ADD PRIMARY KEY (`ID_control`),
  ADD KEY `ID_esp32` (`ID_esp32`),
  ADD KEY `ID_dispositivo` (`ID_dispositivo`);

--
-- Indices de la tabla `dispositivos`
--
ALTER TABLE `dispositivos`
  ADD PRIMARY KEY (`ID_dispositivo`),
  ADD KEY `ID_tipo` (`ID_tipo`);

--
-- Indices de la tabla `esp32`
--
ALTER TABLE `esp32`
  ADD PRIMARY KEY (`ID_esp32`);

--
-- Indices de la tabla `funciones`
--
ALTER TABLE `funciones`
  ADD PRIMARY KEY (`ID_funcion`);

--
-- Indices de la tabla `login_attemps`
--
ALTER TABLE `login_attemps`
  ADD PRIMARY KEY (`ID_login_attemp`),
  ADD KEY `ID_usuario` (`ID_usuario`);

--
-- Indices de la tabla `permisos`
--
ALTER TABLE `permisos`
  ADD PRIMARY KEY (`ID_permiso`);

--
-- Indices de la tabla `senales_ir`
--
ALTER TABLE `senales_ir`
  ADD PRIMARY KEY (`ID_senal`),
  ADD KEY `ID_dispositivo` (`ID_dispositivo`),
  ADD KEY `ID_funcion` (`ID_funcion`);

--
-- Indices de la tabla `sesiones`
--
ALTER TABLE `sesiones`
  ADD PRIMARY KEY (`ID_sesion`),
  ADD KEY `ID_usuario` (`ID_usuario`);

--
-- Indices de la tabla `tipo_dispositivos`
--
ALTER TABLE `tipo_dispositivos`
  ADD PRIMARY KEY (`ID_tipo`);

--
-- Indices de la tabla `usuarios`
--
ALTER TABLE `usuarios`
  ADD PRIMARY KEY (`ID_usuario`),
  ADD KEY `ID_permiso` (`ID_permiso`),
  ADD KEY `ID_administrador` (`ID_administrador`);

--
-- AUTO_INCREMENT de las tablas volcadas
--

--
-- AUTO_INCREMENT de la tabla `acceso_usuarios`
--
ALTER TABLE `acceso_usuarios`
  MODIFY `ID_acceso` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=8;

--
-- AUTO_INCREMENT de la tabla `control_dispositivos`
--
ALTER TABLE `control_dispositivos`
  MODIFY `ID_control` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=5;

--
-- AUTO_INCREMENT de la tabla `dispositivos`
--
ALTER TABLE `dispositivos`
  MODIFY `ID_dispositivo` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=4;

--
-- AUTO_INCREMENT de la tabla `esp32`
--
ALTER TABLE `esp32`
  MODIFY `ID_esp32` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=4;

--
-- AUTO_INCREMENT de la tabla `funciones`
--
ALTER TABLE `funciones`
  MODIFY `ID_funcion` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=5;

--
-- AUTO_INCREMENT de la tabla `login_attemps`
--
ALTER TABLE `login_attemps`
  MODIFY `ID_login_attemp` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=5;

--
-- AUTO_INCREMENT de la tabla `permisos`
--
ALTER TABLE `permisos`
  MODIFY `ID_permiso` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=4;

--
-- AUTO_INCREMENT de la tabla `senales_ir`
--
ALTER TABLE `senales_ir`
  MODIFY `ID_senal` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=5;

--
-- AUTO_INCREMENT de la tabla `sesiones`
--
ALTER TABLE `sesiones`
  MODIFY `ID_sesion` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=4;

--
-- AUTO_INCREMENT de la tabla `tipo_dispositivos`
--
ALTER TABLE `tipo_dispositivos`
  MODIFY `ID_tipo` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=4;

--
-- AUTO_INCREMENT de la tabla `usuarios`
--
ALTER TABLE `usuarios`
  MODIFY `ID_usuario` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=5;

--
-- Restricciones para tablas volcadas
--

--
-- Filtros para la tabla `acceso_usuarios`
--
ALTER TABLE `acceso_usuarios`
  ADD CONSTRAINT `acceso_usuarios_ibfk_1` FOREIGN KEY (`ID_usuario`) REFERENCES `usuarios` (`ID_usuario`),
  ADD CONSTRAINT `acceso_usuarios_ibfk_2` FOREIGN KEY (`ID_control`) REFERENCES `control_dispositivos` (`ID_control`);

--
-- Filtros para la tabla `control_dispositivos`
--
ALTER TABLE `control_dispositivos`
  ADD CONSTRAINT `control_dispositivos_ibfk_1` FOREIGN KEY (`ID_esp32`) REFERENCES `esp32` (`ID_esp32`),
  ADD CONSTRAINT `control_dispositivos_ibfk_2` FOREIGN KEY (`ID_dispositivo`) REFERENCES `dispositivos` (`ID_dispositivo`);

--
-- Filtros para la tabla `dispositivos`
--
ALTER TABLE `dispositivos`
  ADD CONSTRAINT `dispositivos_ibfk_1` FOREIGN KEY (`ID_tipo`) REFERENCES `tipo_dispositivos` (`ID_tipo`);

--
-- Filtros para la tabla `login_attemps`
--
ALTER TABLE `login_attemps`
  ADD CONSTRAINT `login_attemps_ibfk_1` FOREIGN KEY (`ID_usuario`) REFERENCES `usuarios` (`ID_usuario`);

--
-- Filtros para la tabla `senales_ir`
--
ALTER TABLE `senales_ir`
  ADD CONSTRAINT `senales_ir_ibfk_1` FOREIGN KEY (`ID_dispositivo`) REFERENCES `dispositivos` (`ID_dispositivo`),
  ADD CONSTRAINT `senales_ir_ibfk_2` FOREIGN KEY (`ID_funcion`) REFERENCES `funciones` (`ID_funcion`);

--
-- Filtros para la tabla `sesiones`
--
ALTER TABLE `sesiones`
  ADD CONSTRAINT `sesiones_ibfk_1` FOREIGN KEY (`ID_usuario`) REFERENCES `usuarios` (`ID_usuario`);

--
-- Filtros para la tabla `usuarios`
--
ALTER TABLE `usuarios`
  ADD CONSTRAINT `usuarios_ibfk_1` FOREIGN KEY (`ID_permiso`) REFERENCES `permisos` (`ID_permiso`),
  ADD CONSTRAINT `usuarios_ibfk_2` FOREIGN KEY (`ID_administrador`) REFERENCES `usuarios` (`ID_usuario`);
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
