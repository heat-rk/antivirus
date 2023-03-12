import org.jetbrains.compose.desktop.application.dsl.TargetFormat

plugins {
    kotlin("multiplatform")
    id("org.jetbrains.compose")
}

group = AppConfig.group
version = AppConfig.version

repositories {
    google()
    mavenCentral()
    maven("https://maven.pkg.jetbrains.space/public/p/compose/dev")
}

kotlin {
    jvm {
        compilations.all {
            kotlinOptions.jvmTarget = "11"
        }
        withJava()
    }
    sourceSets {
        val jvmMain by getting {
            dependencies {
                implementation(compose.desktop.currentOs)
                implementation(Dependencies.kodein)
                implementation(Dependencies.decompose)
                implementation(Dependencies.decomposeExtensions)
                implementation(Dependencies.sqliteJdbc)
                implementation(Dependencies.exposedCore)
                implementation(Dependencies.exposedDao)
                implementation(Dependencies.exposedJdbc)
                implementation(Dependencies.immutableCollections)
                implementation(Dependencies.coroutinesSwing)
                implementation(files(Dependencies.Files.jnaCore))
                implementation(files(Dependencies.Files.jnaPlatform))
            }
        }
        val jvmTest by getting
    }
}

compose.desktop {
    application {
        javaHome = AppConfig.javaHome
        mainClass = AppConfig.mainClass

        nativeDistributions {
            targetFormats(TargetFormat.Exe)
            packageName = AppConfig.packageName
            packageVersion = AppConfig.version

            windows {
                packageVersion = AppConfig.version
                msiPackageVersion = AppConfig.version
                exePackageVersion = AppConfig.version
            }
        }
    }
}
