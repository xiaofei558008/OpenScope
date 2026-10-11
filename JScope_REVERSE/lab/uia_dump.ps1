param([string]$Name = "Create new project", [int]$Depth = 4)
Add-Type -AssemblyName UIAutomationClient
Add-Type -AssemblyName UIAutomationTypes
$root = [System.Windows.Automation.AutomationElement]::RootElement
$cond = New-Object System.Windows.Automation.PropertyCondition([System.Windows.Automation.AutomationElement]::NameProperty, $Name)
$win = $root.FindFirst([System.Windows.Automation.TreeScope]::Children, $cond)
if ($win -eq $null) { Write-Host "not found: $Name"; exit 1 }
$r = $win.Current.BoundingRectangle
Write-Host ("window '{0}' rect=({1},{2}) {3}x{4}" -f $win.Current.Name, [int]$r.X, [int]$r.Y, [int]$r.Width, [int]$r.Height)
function Walk($el, [int]$d) {
  if ($d -gt $Depth) { return }
  $walker = [System.Windows.Automation.TreeWalker]::ControlViewWalker
  $c = $walker.GetFirstChild($el)
  while ($c -ne $null) {
    $r = $c.Current.BoundingRectangle
    $pad = "  " * $d
    Write-Host ("{0}{1,-14} '{2}' @({3},{4}) {5}x{6} {7}" -f $pad, $c.Current.ControlType.ProgrammaticName.Replace("ControlType.",""), $c.Current.Name, [int]$r.X, [int]$r.Y, [int]$r.Width, [int]$r.Height, $c.Current.AutomationId)
    Walk $c ($d + 1)
    $c = $walker.GetNextSibling($c)
  }
}
Walk $win 1
