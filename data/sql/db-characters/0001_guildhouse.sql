DROP TABLE IF EXISTS `guildhouse`;
CREATE TABLE `guildhouse`
(
    `guildId` INT UNSIGNED NOT NULL,
    `ownerGuid` INT UNSIGNED NOT NULL,
    `locationId` INT UNSIGNED NOT NULL DEFAULT 0,
    `purchaseDate` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,

    PRIMARY KEY (`guildId`),
    KEY `idx_owner` (`ownerGuid`)
) ENGINE=InnoDB
DEFAULT CHARSET=utf8mb4
COLLATE=utf8mb4_unicode_ci;

DROP TABLE IF EXISTS `guildhouse_asset`;
CREATE TABLE `guildhouse_asset`
(
    `assetId` INT UNSIGNED NOT NULL AUTO_INCREMENT,
    `guildId` INT UNSIGNED NOT NULL,
    `layoutId` SMALLINT UNSIGNED NOT NULL DEFAULT 1,
    `catalogId` INT UNSIGNED NOT NULL,
    `status` TINYINT UNSIGNED NOT NULL DEFAULT 0,

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
    `guildId` INT UNSIGNED NOT NULL,
    `instanceId` INT UNSIGNED NOT NULL,

    PRIMARY KEY (`guildId`),
    UNIQUE KEY `idx_instance` (`instanceId`)
) ENGINE=InnoDB
DEFAULT CHARSET=utf8mb4
COLLATE=utf8mb4_unicode_ci;

DROP TABLE IF EXISTS `guildhouse_instance_object`;
CREATE TABLE `guildhouse_instance_object`
(
    `id` BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,

    `instanceId` INT UNSIGNED NOT NULL,
    `guildId` INT UNSIGNED NOT NULL,
    `assetId` INT UNSIGNED NOT NULL,

    `spawnGuid` INT UNSIGNED NOT NULL,
    `spawnType` TINYINT UNSIGNED NOT NULL, -- 0 = Creature, 1 = GameObject

    `mapId` INT UNSIGNED NOT NULL,

    `positionX` FLOAT NOT NULL,
    `positionY` FLOAT NOT NULL,
    `positionZ` FLOAT NOT NULL,
    `orientation` FLOAT NOT NULL,

    PRIMARY KEY (`id`),

    KEY `idx_instance` (`instanceId`),
    KEY `idx_guild` (`guildId`),
    KEY `idx_asset` (`assetId`),
    KEY `idx_spawnGuid` (`spawnGuid`),
    KEY `idx_map_instance` (`mapId`, `instanceId`)
) ENGINE=InnoDB
DEFAULT CHARSET=utf8mb4
COLLATE=utf8mb4_unicode_ci;

DROP TABLE IF EXISTS `guildhouse_salesman`;
CREATE TABLE `guildhouse_salesman`
(
    `guildId` INT UNSIGNED NOT NULL,
    `spawnGuid` INT UNSIGNED NOT NULL,

    `mapId` INT UNSIGNED NOT NULL,
    `instanceId` INT UNSIGNED NOT NULL,

    `positionX` FLOAT NOT NULL,
    `positionY` FLOAT NOT NULL,
    `positionZ` FLOAT NOT NULL,
    `orientation` FLOAT NOT NULL,

    PRIMARY KEY (`guildId`)
) ENGINE=InnoDB
DEFAULT CHARSET=utf8mb4
COLLATE=utf8mb4_unicode_ci;
