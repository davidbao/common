/*
 Navicat Premium Data Transfer

 Source Server         : localhost
 Source Server Type    : MySQL
 Source Server Version : 80013
 Source Host           : localhost:3306
 Source Schema         : tserver

 Target Server Type    : MySQL
 Target Server Version : 80013
 File Encoding         : 65001

 Date: 20/11/2023 09:48:51
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for catalog_user
-- ----------------------------
DROP TABLE IF EXISTS `catalog_user`;
CREATE TABLE `catalog_user` (
  `id` bigint(20) NOT NULL,
  `name` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `password` varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `update_user` varchar(128) DEFAULT NULL,
  `update_time` datetime DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of catalog_user
-- ----------------------------
BEGIN;
INSERT INTO `catalog_user` VALUES (1, 'admin', '4bXMiNc+1YZ81tzpD2ZBkWjADzejfvbLC4vpZLQo0iQ81LL9APomCJJi3Vx8iRO1', NULL, NULL);
INSERT INTO `catalog_user` VALUES (2, 'user', 'jw5chyjSvQQdjAxA3CMPhYTcKbGoRLrfW2CKsIiGAAL6tryNM0/1loiyubDfwrWZ', 'user', '2023-11-20 09:48:20');
COMMIT;

SET FOREIGN_KEY_CHECKS = 1;
