// Need to add properties to the spans to select them in CSS.

$.add_icon_classes = function()
{
    $('span.icon').each(function() { $(this).addClass("icon-" + $(this).text()) })
}
