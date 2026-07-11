DROP TABLE IF EXISTS `guildhouse_locations`;
CREATE TABLE `guildhouse_locations`
(
    `id` INT UNSIGNED NOT NULL AUTO_INCREMENT,
    `name` VARCHAR(100) NOT NULL,

    `mapId` SMALLINT UNSIGNED NOT NULL,
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

    PRIMARY KEY(`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

INSERT INTO `guildhouse_locations` (`name`,`mapId`,`positionX`,`positionY`,`positionZ`,`orientation`,`minX`,`maxX`,`minY`,`maxY`,`price`,`enabled`) VALUES
('GM Island', 1, 16222.57, 16265.91, 13.21, 0, 16000, 16500, 16000, 16500, 100000000, 1),
('Valormok', 37, 88.38, 915.82, 338.12, 0, 16000, 16500, 16000, 16500, 10000000, 1),
('Talrendis Outpost', 37, 1144.43, 240, 353, 0, 16000, 16500, 16000, 16500, 10000000, 1);
