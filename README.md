# ProjectKR

오픈월드 기반 게임을 위한 UE5 자체 플러그인 프레임워크(SeedExt_Core)를 직접 설계하면서, 그 위에 게임 프로젝트(ProjectKR)를 쌓는 구조로 작업 중입니다.

SeedExt_Core 플러그인 (독자 설계)
서브시스템 자동 수집/Tick 관리 프레임워크, 타입 안전 Handle 시스템 (매크로 기반 자동 생성), 위도/계절 기반 물리 태양 시뮬레이션 (일출·일몰 색온도, 레일리 산란), 날씨·바이옴 영향 상태 계산 시스템을 구현 중입니다.

ProjectKR 게임 레이어
Enhanced Input 기반 입력 시스템, 카메라 서브시스템 (외부 카메라 전환 지원) 등으로 실제 플레이 기반이 됩니다.

ProjectKRClient 게임 레이어
CommonUI를 통한 UI 작업을 진행합니다. 추후 Dedicated Server을 위해 ProjectKR와 레이어를 나누었습니다.

ProjectKREditor 게임 레이어
Editor 기능 추가를 위한 레이어입니다. 현재 nif -> fbx를 통한 import Tool을 제작하고 있습니다.
