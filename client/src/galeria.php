<?php
$folder = "../res/";
$allowed = ["jpg", "jpeg", "png", "gif", "webp", "svg"];

$files = array_filter(scandir($folder), function ($file) use ($folder, $allowed) {
    $path = $folder . $file;

    if (!is_file($path)) {
        return false;
    }

    $ext = strtolower(pathinfo($file, PATHINFO_EXTENSION));

    return in_array($ext, $allowed);
});
?>

<!DOCTYPE html>
<html lang="pl">
<head>
    <meta charset="UTF-8">
    <title>Galeria</title>
    <style>
        html, body{
             min-height: 100vh;
            margin: 0;
            font-family: Arial, sans-serif;
            background:
                radial-gradient(circle at top left, rgba(130, 87, 255, 0.35), transparent 35%),
                radial-gradient(circle at top right, rgba(0, 220, 255, 0.25), transparent 30%),
                linear-gradient(135deg, #0f1020 0%, #17172f 45%, #090913 100%);
            color: #f5f5f5;
        }
        body {
            font-family: Arial, sans-serif;
            padding: 20px;
        }

        .gallery {
            display: grid;
            grid-template-columns: repeat(auto-fill, minmax(30px, 1fr));
            gap: 10px;
        }

        .gallery img {
            filter: invert(1);
            height: 30px;
        }

        html,
        body {
            cursor: url("../res/cursorPointer.svg") 0 0, auto;
        }
        a,
        button,
        [role="button"],
        .item {
            cursor: url("../res/cursorClick.svg") 8 0, pointer;
        }
        .draggable {
            cursor: url("../res/cursorGrab.svg") 8 8, grab;
        }
    </style>
</head>
<body>

<div class="gallery">
    <?php foreach ($files as $file): ?>
    <img 
        class="draggable"
        src="<?= htmlspecialchars($folder . rawurlencode($file)) ?>?v=<?= filemtime($folder . $file) ?>" 
        alt="<?= htmlspecialchars($file) ?>"
    >
    <?php endforeach; ?>
</div>

</body>
</html>