package com.ffpanel.panel

import android.os.Bundle
import android.widget.SeekBar
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import com.ffpanel.panel.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding
    private var uptimeSeconds = 0
    private val handler = android.os.Handler(android.os.Looper.getMainLooper())

    // Chamadas nativas C++ - Features
    external fun getFeatureCount(): Int
    external fun getFeatureName(index: Int): String
    external fun getFeatureDesc(index: Int): String
    external fun isFeatureEnabled(index: Int): Boolean
    external fun setFeatureEnabled(index: Int, enabled: Boolean)
    external fun getActiveModsCount(): Int

    // ESP Main
    external fun isESPEnabled(): Boolean

    // ESP Individual Toggles
    external fun isBoxESPEnabled(): Boolean
    external fun setBoxESPEnabled(enabled: Boolean)
    external fun isLineESPEnabled(): Boolean
    external fun setLineESPEnabled(enabled: Boolean)
    external fun isSkeletonESPEnabled(): Boolean
    external fun setSkeletonESPEnabled(enabled: Boolean)
    external fun isNameESPEnabled(): Boolean
    external fun setNameESPEnabled(enabled: Boolean)
    external fun isGlowESPEnabled(): Boolean
    external fun setGlowESPEnabled(enabled: Boolean)
    external fun isCorneredESPEnabled(): Boolean
    external fun setCorneredESPEnabled(enabled: Boolean)

    // ESP Enemy Data
    external fun getEnemyCount(): Int
    external fun getEnemyX(index: Int): Float
    external fun getEnemyY(index: Int): Float
    external fun getEnemyWidth(index: Int): Float
    external fun getEnemyHeight(index: Int): Float
    external fun getEnemyDistance(index: Int): Float
    external fun getEnemyHealth(index: Int): Float
    external fun isEnemyVisible(index: Int): Boolean
    external fun getEnemyName(index: Int): String
    external fun getActiveESPCount(): Int

    // ESP Color & Settings
    external fun getESPR(): Float
    external fun getESPG(): Float
    external fun getESPB(): Float
    external fun setESPColor(r: Float, g: Float, b: Float)
    external fun getESPGlowIntensity(): Float
    external fun setESPGlowIntensity(intensity: Float)
    external fun getESPThickness(): Float
    external fun setESPThickness(thickness: Float)

    // Settings
    external fun getSensitivity(): Float
    external fun setSensitivity(value: Float)
    external fun getAimIntensity(): Float
    external fun setAimIntensity(value: Float)
    external fun getModSpeed(): Float
    external fun setModSpeed(value: Float)
    external fun activateAll()
    external fun deactivateAll()
    external fun resetAll()
    external fun isMasterEnabled(): Boolean

    init { System.loadLibrary("ffpanel") }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)
        setupUI()
        startUptime()
    }

    private fun setupUI() {
        // Feature toggles (0-3: aim, recoil, speed, fly)
        binding.switchAim.setOnCheckedChangeListener { _, isChecked -> setFeatureEnabled(0, isChecked); updateStats(); toast("AIM ${if(isChecked) "ON" else "OFF"}") }
        binding.switchRecoil.setOnCheckedChangeListener { _, isChecked -> setFeatureEnabled(1, isChecked); updateStats(); toast("NO RECOIL ${if(isChecked) "ON" else "OFF"}") }
        binding.switchSpeed.setOnCheckedChangeListener { _, isChecked -> setFeatureEnabled(2, isChecked); updateStats(); toast("SPEED ${if(isChecked) "ON" else "OFF"}") }
        binding.switchFly.setOnCheckedChangeListener { _, isChecked -> setFeatureEnabled(3, isChecked); updateStats(); toast("FLY ${if(isChecked) "ON" else "OFF"}") }
        
        // ESP Main Toggle
        binding.switchEsp.setOnCheckedChangeListener { _, isChecked ->
            setFeatureEnabled(4, isChecked)
            updateStats()
            if (isChecked) {
                binding.tvEnemyCount.text = "${getEnemyCount()}"
                binding.tvEspActive.text = "${getActiveESPCount()}/6"
                toast("ESP ON! ${getEnemyCount()} alvos")
            } else {
                binding.tvEnemyCount.text = "0"
                binding.tvEspActive.text = "0/6"
                toast("ESP OFF")
            }
        }

        // ===== ESP Individual Toggles =====
        
        // Box ESP
        binding.switchEspBox.setOnCheckedChangeListener { _, isChecked ->
            setBoxESPEnabled(isChecked)
            updateESPCount()
            toast("Box ESP ${if(isChecked) "ON" else "OFF"}")
        }

        // Line ESP
        binding.switchEspLine.setOnCheckedChangeListener { _, isChecked ->
            setLineESPEnabled(isChecked)
            updateESPCount()
            toast("Line ESP ${if(isChecked) "ON" else "OFF"}")
        }

        // Skeleton ESP
        binding.switchEspSkeleton.setOnCheckedChangeListener { _, isChecked ->
            setSkeletonESPEnabled(isChecked)
            updateESPCount()
            toast("Skeleton ESP ${if(isChecked) "ON" else "OFF"}")
        }

        // Name ESP
        binding.switchEspName.setOnCheckedChangeListener { _, isChecked ->
            setNameESPEnabled(isChecked)
            updateESPCount()
            toast("Name ESP ${if(isChecked) "ON" else "OFF"}")
        }

        // Glow ESP
        binding.switchEspGlow.setOnCheckedChangeListener { _, isChecked ->
            setGlowESPEnabled(isChecked)
            updateESPCount()
            toast("Glow ESP ${if(isChecked) "ON" else "OFF"}")
        }

        // Cornered Box ESP
        binding.switchEspCornered.setOnCheckedChangeListener { _, isChecked ->
            setCorneredESPEnabled(isChecked)
            updateESPCount()
            toast("Cornered Box ${if(isChecked) "ON" else "OFF"}")
        }

        // ESP Color buttons
        binding.btnEspRed.setOnClickListener { setESPColor(1f, 0f, 0f); toast("Cor: Vermelho") }
        binding.btnEspGreen.setOnClickListener { setESPColor(0f, 1f, 0f); toast("Cor: Verde") }
        binding.btnEspBlue.setOnClickListener { setESPColor(0f, 0.5f, 1f); toast("Cor: Azul") }
        binding.btnEspYellow.setOnClickListener { setESPColor(1f, 1f, 0f); toast("Cor: Amarelo") }
        binding.btnEspMagenta.setOnClickListener { setESPColor(1f, 0f, 1f); toast("Cor: Magenta") }

        // Glow Intensity Slider
        binding.seekbarGlowIntensity.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            override fun onProgressChanged(sb: SeekBar?, progress: Int, fromUser: Boolean) {
                if (fromUser) { setESPGlowIntensity(progress / 100f); binding.tvGlowValue.text = "$progress%" }
            }
            override fun onStartTrackingTouch(sb: SeekBar?) {}
            override fun onStopTrackingTouch(sb: SeekBar?) {}
        })

        // Thickness Slider
        binding.seekbarEspThickness.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            override fun onProgressChanged(sb: SeekBar?, progress: Int, fromUser: Boolean) {
                if (fromUser) { setESPThickness(progress.toFloat()); binding.tvThicknessValue.text = "${progress}px" }
            }
            override fun onStartTrackingTouch(sb: SeekBar?) {}
            override fun onStopTrackingTouch(sb: SeekBar?) {}
        })

        // Master Toggle
        binding.switchMaster.setOnCheckedChangeListener { _, isChecked ->
            if (isChecked) activateAll() else deactivateAll()
            syncAllSwitches()
            updateStats()
            updateESPCount()
        }

        // Sliders
        binding.seekbarSensitivity.setOnSeekBarChangeListener(sliderListener { progress -> setSensitivity(progress.toFloat()); binding.tvSensValue.text = "$progress%" })
        binding.seekbarAimIntensity.setOnSeekBarChangeListener(sliderListener { progress -> setAimIntensity(progress.toFloat()); binding.tvAimIntensityValue.text = "$progress%" })
        binding.seekbarModSpeed.setOnSeekBarChangeListener(sliderListener { progress -> setModSpeed(progress.toFloat()); binding.tvModSpeedValue.text = "$progress%" })

        // Buttons
        binding.btnActivateAll.setOnClickListener { activateAll(); syncAllSwitches(); updateStats(); updateESPCount(); toast("TODOS ATIVOS!") }
        binding.btnDeactivateAll.setOnClickListener { deactivateAll(); syncAllSwitches(); updateStats(); updateESPCount(); toast("TODOS OFF!") }
        binding.btnReset.setOnClickListener { resetAll(); syncAllSwitches(); resetSliders(); uptimeSeconds = 0; updateStats(); updateESPCount(); toast("RESET OK!") }
        binding.btnSettings.setOnClickListener { toast("Configurações...") }

        updateStats()
        updateESPCount()
    }

    private fun sliderListener(onProgress: (Int) -> Unit) = object : SeekBar.OnSeekBarChangeListener {
        override fun onProgressChanged(sb: SeekBar?, progress: Int, fromUser: Boolean) { if (fromUser) onProgress(progress) }
        override fun onStartTrackingTouch(sb: SeekBar?) {}
        override fun onStopTrackingTouch(sb: SeekBar?) {}
    }

    private fun syncAllSwitches() {
        binding.switchAim.isChecked = isFeatureEnabled(0)
        binding.switchRecoil.isChecked = isFeatureEnabled(1)
        binding.switchSpeed.isChecked = isFeatureEnabled(2)
        binding.switchFly.isChecked = isFeatureEnabled(3)
        binding.switchEsp.isChecked = isFeatureEnabled(4)
        binding.switchEspBox.isChecked = isBoxESPEnabled()
        binding.switchEspLine.isChecked = isLineESPEnabled()
        binding.switchEspSkeleton.isChecked = isSkeletonESPEnabled()
        binding.switchEspName.isChecked = isNameESPEnabled()
        binding.switchEspGlow.isChecked = isGlowESPEnabled()
        binding.switchEspCornered.isChecked = isCorneredESPEnabled()
        binding.switchMaster.isChecked = isMasterEnabled()
    }

    private fun updateESPCount() {
        binding.tvEspActive.text = "${getActiveESPCount()}/6"
    }

    private fun resetSliders() {
        binding.seekbarSensitivity.progress = 50
        binding.seekbarAimIntensity.progress = 75
        binding.seekbarModSpeed.progress = 100
        binding.seekbarGlowIntensity.progress = 50
        binding.seekbarEspThickness.progress = 2
        binding.tvSensValue.text = "50%"
        binding.tvAimIntensityValue.text = "75%"
        binding.tvModSpeedValue.text = "100%"
        binding.tvGlowValue.text = "50%"
        binding.tvThicknessValue.text = "2px"
    }

    private fun updateStats() {
        val cnt = getActiveModsCount()
        binding.tvActiveMods.text = "$cnt/5"
        binding.tvStatus.text = if (cnt > 0) "ONLINE" else "STANDBY"
        binding.tvPrecision.text = if (cnt > 0) "94.7%" else "0%"
    }

    private fun startUptime() {
        val r = object : Runnable {
            override fun run() {
                uptimeSeconds++
                val h = uptimeSeconds / 3600
                val m = (uptimeSeconds % 3600) / 60
                val s = uptimeSeconds % 60
                binding.tvUptime.text = String.format("%02d:%02d:%02d", h, m, s)
                handler.postDelayed(this, 1000)
            }
        }
        handler.postDelayed(r, 1000)
    }

    private fun toast(msg: String) = Toast.makeText(this, msg, Toast.LENGTH_SHORT).show()
    override fun onDestroy() { super.onDestroy(); handler.removeCallbacksAndMessages(null) }
}
