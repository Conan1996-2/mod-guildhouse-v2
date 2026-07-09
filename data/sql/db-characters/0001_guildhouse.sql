DROP TABLE IF EXISTS `guildhouse`;
CREATE TABLE `guildhouse`
(
    `guildId` INT UNSIGNED NOT NULL,
    `ownerGuid` INT UNSIGNED NOT NULL,
    `purchaseDate` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (`guildId`),
    KEY `idx_owner` (`ownerGuid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

DROP TABLE IF EXISTS `guildhouse_asset`;
CREATE TABLE `guildhouse_asset`
(
    `assetId` INT UNSIGNED NOT NULL AUTO_INCREMENT,
    `guildId` INT UNSIGNED NOT NULL,
    `layoutId` SMALLINT UNSIGNED NOT NULL DEFAULT 1,
    `catalogId` INT UNSIGNED NOT NULL,
    `status` TINYINT UNSIGNED NOT NULL DEFAULT 0,
    `phase` INT UNSIGNED NOT NULL,
    `positionX` FLOAT NOT NULL,
    `positionY` FLOAT NOT NULL,
    `positionZ` FLOAT NOT NULL,
    `orientation` FLOAT NOT NULL,
    `createdBy` INT UNSIGNED NOT NULL,
    `enabled` TINYINT(1) NOT NULL DEFAULT 1,
    `createdDate` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (`assetId`),
    KEY `idx_guild` (`guildId`),
    KEY `idx_layout` (`layoutId`),
    KEY `idx_catalog` (`catalogId`),
    KEY `idx_status` (`status`)
) ENGINE=InnoDB
DEFAULT CHARSET=utf8mb4
COLLATE=utf8mb4_unicode_ci;

DROP TABLE IF EXISTS `guildhouse_instance`;
CREATE TABLE `guildhouse_instance`
(
    `id` BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
    `guildId` INT UNSIGNED NOT NULL,
    `assetId` INT UNSIGNED NOT NULL,
    `catalogId` INT UNSIGNED NOT NULL,
    `guid` INT UNSIGNED NOT NULL,
    `type` TINYINT UNSIGNED NOT NULL, -- 0 creature, 1 gameobject
    `mapId` INT UNSIGNED NOT NULL,
    `phase` INT UNSIGNED NOT NULL,
    `x` FLOAT NOT NULL,
    `y` FLOAT NOT NULL,
    `z` FLOAT NOT NULL,
    `o` FLOAT NOT NULL,
    PRIMARY KEY (`id`),
    KEY `idx_guild` (`guildId`),
    KEY `idx_phase` (`phase`),
    KEY `idx_map_phase` (`mapId`, `phase`),
    KEY `idx_asset` (`assetId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

DROP TABLE IF EXISTS `guildhouse_spawn`;
CREATE TABLE `guildhouse_spawn`
(
    `spawnId` BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
    `assetId` INT UNSIGNED NOT NULL,
    `guildId` INT UNSIGNED NOT NULL,
    `spawnGuid` INT UNSIGNED NOT NULL,
    `spawnType` TINYINT UNSIGNED NOT NULL,
    `mapId` INT UNSIGNED NOT NULL,
    `phase` INT UNSIGNED NOT NULL,
    `positionX` FLOAT NOT NULL,
    `positionY` FLOAT NOT NULL,
    `positionZ` FLOAT NOT NULL,
    `orientation` FLOAT NOT NULL,
    `enabled` TINYINT(1) NOT NULL DEFAULT 1,
    `createdDate` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (`spawnId`),
    KEY `idx_asset` (`assetId`),
    KEY `idx_guild` (`guildId`),
    KEY `idx_spawnGuid` (`spawnGuid`),
    KEY `idx_enabled` (`enabled`)
) ENGINE=InnoDB
DEFAULT CHARSET=utf8mb4
COLLATE=utf8mb4_unicode_ci;

DROP TABLE IF EXISTS `guildhouse_salesman`;
CREATE TABLE `guildhouse_salesman`
(
    `guildId` INT UNSIGNED NOT NULL,
    `guid` INT UNSIGNED NOT NULL,
    `mapId` INT UNSIGNED NOT NULL,
    `phase` INT UNSIGNED NOT NULL,
    `positionX` FLOAT NOT NULL,
    `positionY` FLOAT NOT NULL,
    `positionZ` FLOAT NOT NULL,
    `orientation` FLOAT NOT NULL,
    PRIMARY KEY (`guildId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
