DROP TABLE IF EXISTS `guildhouse_category`;
CREATE TABLE `guildhouse_category`
(
    `categoryId` INT UNSIGNED NOT NULL AUTO_INCREMENT,
    `parentId` INT UNSIGNED NOT NULL DEFAULT 0,
    `name` VARCHAR(80) NOT NULL,
    `sortOrder` SMALLINT UNSIGNED NOT NULL DEFAULT 0,
    `enabled` TINYINT(1) NOT NULL DEFAULT 1,
    PRIMARY KEY (`categoryId`),
    KEY `idx_parent` (`parentId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

DROP TABLE IF EXISTS `guildhouse_catalog`;
CREATE TABLE `guildhouse_catalog`
(
    `catalogId` INT UNSIGNED NOT NULL AUTO_INCREMENT,
    `categoryId` INT UNSIGNED NOT NULL,
    `name` VARCHAR(100) NOT NULL,
    `spawnFlags` INT UNSIGNED NOT NULL DEFAULT 0,
    `behaviorFlags` INT UNSIGNED NOT NULL DEFAULT 0,
    `enabled` TINYINT(1) NOT NULL DEFAULT 1,
    PRIMARY KEY (`catalogId`),
    KEY `idx_category` (`categoryId`),
    KEY `idx_enabled` (`enabled`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

DROP TABLE IF EXISTS `guildhouse_catalog_asset`;
CREATE TABLE `guildhouse_catalog_asset`
(
    `componentId` INT UNSIGNED NOT NULL AUTO_INCREMENT,
    `catalogId` INT UNSIGNED NOT NULL,
    `spawnFlags` INT UNSIGNED NOT NULL DEFAULT 0,
    `behaviorFlags` INT UNSIGNED NOT NULL DEFAULT 0,
    `entry` INT UNSIGNED NOT NULL DEFAULT 0,
    `displayId` INT UNSIGNED NOT NULL DEFAULT 0,
    `scale` FLOAT NOT NULL DEFAULT 1.0,
    `scriptType` SMALLINT UNSIGNED NOT NULL DEFAULT 0,
    `scriptData` VARCHAR(100) DEFAULT NULL,
    `xOffset` FLOAT NOT NULL DEFAULT 0,
    `yOffset` FLOAT NOT NULL DEFAULT 0,
    `zOffset` FLOAT NOT NULL DEFAULT 0,
    `oOffset` FLOAT NOT NULL DEFAULT 0,
    `targetMap` SMALLINT UNSIGNED DEFAULT NULL,
    `targetX` FLOAT DEFAULT NULL,
    `targetY` FLOAT DEFAULT NULL,
    `targetZ` FLOAT DEFAULT NULL,
    `targetO` FLOAT DEFAULT NULL,
    `childCatalogId` INT UNSIGNED DEFAULT NULL,
    `sortOrder` SMALLINT UNSIGNED NOT NULL DEFAULT 0,
    PRIMARY KEY (`componentId`),
    KEY `idx_catalog` (`catalogId`),
    KEY `idx_child` (`childCatalogId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
