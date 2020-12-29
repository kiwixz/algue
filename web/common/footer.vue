<template>
  <div class="footer">
    <div class="spacing"></div>
    <div class="bottom">
      algue version {{ version }} for League of Legends {{ riot_version }} (source on
      <a href="https://github.com/kiwixz/algue">github</a>)
    </div>
  </div>
</template>

<style lang="scss">
  .footer {
    color: #242;
    font-size: 0.75em;

    .spacing, .bottom {
      padding: 5px;
    }

    .spacing {
      height: calc(1em + 80px);
    }

    .bottom {
      bottom: 0;
      position: fixed;
      right: 0;

      background-color: #000d;
    }
  }
</style>

<script lang="ts">
  import Vue from "vue";
  import Component from "vue-class-component";

  import Info from "Generated/info";
  import RiotInfo from "Riot/info";

  @Component
  export default class extends Vue {
    get version() {
      return Info.version;
    }

    get riot_version() {
      return RiotInfo.version;
    }
  }
</script>
