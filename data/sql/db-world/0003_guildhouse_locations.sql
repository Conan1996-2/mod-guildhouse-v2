DROP TABLE IF EXISTS `guildhouse_locations`;
CREATE TABLE `guildhouse_locations`
(
    `id` INT UNSIGNED NOT NULL AUTO_INCREMENT,
    `name` VARCHAR(100) NOT NULL,

    `mapId` SMALLINT UNSIGNED NOT NULL,
    `zoneId` SMALLINT UNSIGNED NOT NULL,
    `areaId` SMALLINT UNSIGNED NOT NULL,
    `positionX` FLOAT NOT NULL,
    `positionY` FLOAT NOT NULL,
    `positionZ` FLOAT NOT NULL,
    `orientation` FLOAT NOT NULL,

    `minX` FLOAT NOT NULL,
    `maxX` FLOAT NOT NULL,
    `minY` FLOAT NOT NULL,
    `maxY` FLOAT NOT NULL,

    `price` BIGINT UNSIGNED NOT NULL DEFAULT 0,
    `enabled` TINYINT(1) NOT NULL DEFAULT 1,

    PRIMARY KEY(`id`),
    KEY `idx_map` (`mapId`),
    KEY `idx_enabled` (`enabled`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
