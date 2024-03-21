import QtQuick

Canvas {
    LoggingCategory {
        id: tag
        name: "LineMask"
    }

    onPaint: {
        console.log(tag, "Canvas.onPaint")
        let ctx = getContext("2d")
        ctx.clearRect(0, 0, width, height)
        ctx.lineWidth = 0.5
        ctx.beginPath() // 开始一条路径
        ctx.moveTo(width * 0.5, height * 0.2) // 移动到指定位置
        ctx.lineTo(width * 0.5, height)
        ctx.stroke()
    }
}
