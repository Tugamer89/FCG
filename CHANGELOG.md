# Changelog

## [2.11.0](https://github.com/Tugamer89/FCG/compare/v2.10.1...v2.11.0) (2026-05-25)


### Features

* **Lab6:** Refactor shader code to use structured Light and Material types ([a16234b](https://github.com/Tugamer89/FCG/commit/a16234b34a76f3d9db18eb4d0bf8d2c339b78143))


### Bug Fixes

* **Lab6:** pdf track ([318f5ee](https://github.com/Tugamer89/FCG/commit/318f5ee72ecf2bb54357cb07263d3d402466eb15))

## [2.10.1](https://github.com/Tugamer89/FCG/compare/v2.10.0...v2.10.1) (2026-05-25)


### Bug Fixes

* **Lab6:** flat shader ([1eb07c8](https://github.com/Tugamer89/FCG/commit/1eb07c82f774691466d799eddd5b66cda28fc86f))

## [2.10.0](https://github.com/Tugamer89/FCG/compare/v2.9.0...v2.10.0) (2026-05-20)


### Features

* **Lab6:** add Gouraud, Phong, and Flat shading implementations with corresponding shaders ([cadc4f3](https://github.com/Tugamer89/FCG/commit/cadc4f308db1e30dbe4af1ce9cd39eed0a575046))
* **Lab6:** added the base for new Lab ([1d23418](https://github.com/Tugamer89/FCG/commit/1d234186c53443312092ce2ecf0e75cdfa0e854a))
* **Lab6:** enhance shading with Gouraud shading implementation and update shader code ([cde7d66](https://github.com/Tugamer89/FCG/commit/cde7d6671b5069a396c1ccd741d444f0a2702f39))
* **Lab6:** implement mesh normal computation and update shaders ([e7f29a1](https://github.com/Tugamer89/FCG/commit/e7f29a1f8e001cb1cd8118981562c8dc8e43f416))


### Code Refactoring

* **cmake:** simplify lab targets and globalize common settings ([0704d0d](https://github.com/Tugamer89/FCG/commit/0704d0d5c617afacacbe3ce6d39aee2614cbb0d4))

## [2.9.0](https://github.com/Tugamer89/FCG/compare/v2.8.0...v2.9.0) (2026-05-19)


### Features

* add pyramid mesh and SonarQube configuration ([9daa23e](https://github.com/Tugamer89/FCG/commit/9daa23edf6d0f3a9183a08d50d5d74d3e61b7633))
* add SonarCloud configuration and improve project structure ([ac83238](https://github.com/Tugamer89/FCG/commit/ac8323833a1a3d67b7a01ecbc8718e99918c0ef9))


### Bug Fixes

* add missing dependencies for C++ compiler setup ([8270d0a](https://github.com/Tugamer89/FCG/commit/8270d0a366ea6c306a6d4335cf580ae07369a6df))
* **ci:** add SonarCloud configuration file ([efcdabc](https://github.com/Tugamer89/FCG/commit/efcdabcd04fad90dc556f40b20d5d5dfa5ef6a88))
* correct sonar.exclusions placement in configuration file ([edc69fe](https://github.com/Tugamer89/FCG/commit/edc69fea4f5a6267f4aaeb4dcaeddbb4739e617d))
* downgrade C++ standard in SonarCloud configuration for compatibility ([5a6f287](https://github.com/Tugamer89/FCG/commit/5a6f28764f63133b48aa3382acb76c755f77ea0d))
* update resource paths and improve code consistency across multiple files ([b14ee2f](https://github.com/Tugamer89/FCG/commit/b14ee2f42fd58ac2b1460acc7e7c571c89df8969))
* update SonarCloud configuration for build environment and source inclusion ([d55e143](https://github.com/Tugamer89/FCG/commit/d55e14389780821bdc07c97f8178e03d14fc100b))


### Code Refactoring

* use 'auto' for type deduction in Camera class update method ([6ce56b6](https://github.com/Tugamer89/FCG/commit/6ce56b6435e2c94a7030e322ae87f4c3992a5074))

## [2.8.0](https://github.com/Tugamer89/FCG/compare/v2.7.0...v2.8.0) (2026-05-18)


### Features

* **Lab5:** animation ([a27dc4c](https://github.com/Tugamer89/FCG/commit/a27dc4c48c7f9bf4d8a061bf23cfa0302f92677d))


### Bug Fixes

* streamline key handling for camera reset in event processing ([0527e8c](https://github.com/Tugamer89/FCG/commit/0527e8cfc95b3c1ac820a870099d4ef72944fcca))

## [2.7.0](https://github.com/Tugamer89/FCG/compare/v2.6.0...v2.7.0) (2026-05-15)


### Features

* **Lab5:** add glm dependency and update shader handling for camera transformations ([5d89cb3](https://github.com/Tugamer89/FCG/commit/5d89cb3ba58ff69f2fc121c3064a50f1bb0c7fa0))
* **Lab5:** added first steps ([cb05023](https://github.com/Tugamer89/FCG/commit/cb05023b77e9ffa6bc6b1ad161fb9b948170fd3a))
* **Lab5:** ended Lab with optional features ([43b7db9](https://github.com/Tugamer89/FCG/commit/43b7db9ef18377e096dddfb9ff17cbece761872c))
* **Lab5:** implement camera controls and update shaders for rotation ([611cbba](https://github.com/Tugamer89/FCG/commit/611cbbaef3330d940ab23a98af7bca8fa6c06eac))


### Code Refactoring

* **Scene:** remove unused reload function ([0a35830](https://github.com/Tugamer89/FCG/commit/0a358301cfa2f878671acb41d6d78fc3a7cf65e8))

## [2.6.0](https://github.com/Tugamer89/FCG/compare/v2.5.0...v2.6.0) (2026-05-12)


### Features

* **cmake:** automate shader copying to output directory ([2c2c4f1](https://github.com/Tugamer89/FCG/commit/2c2c4f1735042e16a764c716a5ecb39189efe383))


### Bug Fixes

* **cmake:** statically link mingw libraries on windows ([35ca94b](https://github.com/Tugamer89/FCG/commit/35ca94ba20ef7d8239719ef969aa2d3021fe6406))

## [2.5.0](https://github.com/Tugamer89/FCG/compare/v2.4.0...v2.5.0) (2026-05-11)


### Features

* **Lab4:** added new steps ([9290670](https://github.com/Tugamer89/FCG/commit/929067068701db5b83f09340c12d705c6aae1df5))

## [2.4.0](https://github.com/Tugamer89/FCG/compare/v2.3.0...v2.4.0) (2026-05-05)


### Features

* added alternating shader ([f31554b](https://github.com/Tugamer89/FCG/commit/f31554b7fc1d67e5fabcdb5b1edcfedcbc9356c0))
* **Lab4:** added base code for part 2 ([7b3fdf0](https://github.com/Tugamer89/FCG/commit/7b3fdf054253b5b8e6b13507d0928de6e3029fad))


### Bug Fixes

* **ci:** added .vert and .frag to formatter ([83fe46b](https://github.com/Tugamer89/FCG/commit/83fe46b972904bd4a20b3ebb5f642f6dd837e348))
* **ci:** fixed double uploading ([9e3f834](https://github.com/Tugamer89/FCG/commit/9e3f8343a022900132bc3b116ceebc22f7ca3cc5))
* **ci:** improved code ql analysis ([67bd345](https://github.com/Tugamer89/FCG/commit/67bd345f103121d233b62108aa2acc2dc606f1c7))
* **ci:** improved codeql analysis ([bf45885](https://github.com/Tugamer89/FCG/commit/bf458855858b69e04bb8f84c11d5c542799ece52))
* **ci:** improved linux requirements installation ([3ad9523](https://github.com/Tugamer89/FCG/commit/3ad9523472259721fbbf3980d9f1abc3dae78b89))
* **ci:** improved requirements installation ([fa9e62c](https://github.com/Tugamer89/FCG/commit/fa9e62c10a2dc82ab7286a6df35901eb19e8083e))

## [2.3.0](https://github.com/Tugamer89/FCG/compare/v2.2.0...v2.3.0) (2026-04-28)


### Features

* added trajectory rewind ([3716409](https://github.com/Tugamer89/FCG/commit/37164091383109773fb5828fefbd4d704d1c75d2))


### Code Refactoring

* **lab3:** implement AABB collision detection ([1a896e6](https://github.com/Tugamer89/FCG/commit/1a896e670776b4c4f7bb3871d025c25ed7f1386a))

## [2.2.0](https://github.com/Tugamer89/FCG/compare/v2.1.1...v2.2.0) (2026-04-27)


### Features

* added shaders ([2511821](https://github.com/Tugamer89/FCG/commit/25118214e033384944b9fdd450f38c37194383b5))
* Lab4 initial commit ([e89ddfa](https://github.com/Tugamer89/FCG/commit/e89ddfaa6d8c9fa99fb064bb2b51ac0237eb5aa7))

## [2.1.1](https://github.com/Tugamer89/FCG/compare/v2.1.0...v2.1.1) (2026-04-26)


### Bug Fixes

* **paddle:** correct normalized offset logic for ball bounce ([72ff2e9](https://github.com/Tugamer89/FCG/commit/72ff2e9972f9e6d738c38e6844b2965bd7e08fd9))

## [2.1.0](https://github.com/Tugamer89/FCG/compare/v2.0.1...v2.1.0) (2026-04-26)


### Features

* Improved ball physics, scoring logic, and levels ([6c54d19](https://github.com/Tugamer89/FCG/commit/6c54d19dcd2fc75b716284be39c85bdfd798db0d))

## [2.0.1](https://github.com/Tugamer89/FCG/compare/v2.0.0...v2.0.1) (2026-04-23)


### Bug Fixes

* added release version to release files ([6ce3e66](https://github.com/Tugamer89/FCG/commit/6ce3e6605b5081c6af32fb1880ad5574e775a7eb))

## [2.0.0](https://github.com/Tugamer89/FCG/compare/v1.1.0...v2.0.0) (2026-04-23)


### ⚠ BREAKING CHANGES

* updated C++ standard to C++26 and updated cmake minimum version to match the requirements

### Features

* added new level ([ac22872](https://github.com/Tugamer89/FCG/commit/ac2287274be4727f902e73b71c06bc6cf7bdca3f))
* Implement automated packaging and optimize CI/CD ([c66466c](https://github.com/Tugamer89/FCG/commit/c66466c62ada240f2fc556106b78b4ec6b9af20d))
* **lab3:** Implements lives, levels, particles, and resources via C++26 #embed ([bbd19c4](https://github.com/Tugamer89/FCG/commit/bbd19c4fd9883a5882787a20780b0584323d519d))
* updated C++ standard to C++26 and updated cmake minimum version to match the requirements ([28dae9f](https://github.com/Tugamer89/FCG/commit/28dae9fc410de04def0ffb6bf9ab404ba2619855))


### Bug Fixes

* added license for the resources ([e332941](https://github.com/Tugamer89/FCG/commit/e3329416bf3d781545c21d4544799f97f73b2a32))
* bumped SFML version ([c6a4ec8](https://github.com/Tugamer89/FCG/commit/c6a4ec83f970b695013e5e08b2147f1ef8b88f50))
* moved set of cxx standard after library inclusion ([187c2f5](https://github.com/Tugamer89/FCG/commit/187c2f5c12cbd7483812bef4226af0dd7fb9bd38))

## [1.1.0](https://github.com/Tugamer89/FCG/compare/v1.0.0...v1.1.0) (2026-04-23)


### Features

* added colors ([1ffeaeb](https://github.com/Tugamer89/FCG/commit/1ffeaeb28f9480d9a24eb8447c9050e80d7006e4))
* added score ([0c49769](https://github.com/Tugamer89/FCG/commit/0c497693880b119602c90c722b7575639326460f))
* finished chapter 4 ([912b3f5](https://github.com/Tugamer89/FCG/commit/912b3f50670ae4d4bd1421ce9885d452c1914ed4))
* first 8 steps ([6e69780](https://github.com/Tugamer89/FCG/commit/6e697803494e534436c1f116d34d55491e3247b7))
* improved code until chapter 3 ([a8de8c3](https://github.com/Tugamer89/FCG/commit/a8de8c3b1d440006064754e384c56c983d8bb03e))
* improved radius calculations ([8ff87c3](https://github.com/Tugamer89/FCG/commit/8ff87c37a6f9d28f1fb484ffcd41134e4a23e682))


### Bug Fixes

* improved code quality ([49bd037](https://github.com/Tugamer89/FCG/commit/49bd0376c63c3a45f8dc20094e4c9eebe1af9a89))
* improved colors & selected shpaes graphics ([d8ed859](https://github.com/Tugamer89/FCG/commit/d8ed85930a928ce4ea8a9b087fb54cff830d39e9))
* refine shape movement and size constraints ([d80f887](https://github.com/Tugamer89/FCG/commit/d80f88772082d6e23ce059907e1f4cbd215fee73))
